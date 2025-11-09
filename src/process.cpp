#include "process.h"
#include <filesystem>
#include <fstream>
#include <pwd.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <map>
#include <limits>


namespace fs = std::filesystem;

static unsigned long long readTotalCpuJiffies() {
    std::ifstream file("/proc/stat");
    std::string cpu;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    file >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    return user + nice + system + idle + iowait + irq + softirq + steal;
}

std::vector<ProcessInfo> getProcesses(SortMode sortMode) {
    static std::map<int, unsigned long long> prevProcTimes;
    static unsigned long long prevTotalJiffies = 0;

    unsigned long long currTotalJiffies = readTotalCpuJiffies();

    std::vector<ProcessInfo> processes;

    for (const auto &entry : fs::directory_iterator("/proc")) {
        if (!entry.is_directory()) continue;
        std::string dirname = entry.path().filename();
        if (!std::all_of(dirname.begin(), dirname.end(), ::isdigit)) continue;

        int pid = std::stoi(dirname);
        std::ifstream statFile("/proc/" + dirname + "/stat");
        std::ifstream statusFile("/proc/" + dirname + "/status");
        if (!statFile || !statusFile) continue;

        std::string comm;
        char state;
        unsigned long utime, stime;
        unsigned long vsize;
        long rss;
        std::string tmp;
        unsigned long starttime;

        // /proc/[pid]/stat structure: https://man7.org/linux/man-pages/man5/proc.5.html
        statFile >> pid >> comm >> state;
        for (int i = 0; i < 10; i++) statFile >> tmp; // skip 10 fields
        statFile >> utime >> stime;                   // fields 14,15
        for (int i = 0; i < 7; i++) statFile >> tmp;  // skip 7 more fields
        statFile >> starttime >> vsize >> rss;

        unsigned long long totalProcJiffies = utime + stime;

        unsigned long long prevProcJiffies = prevProcTimes[pid];
        unsigned long long deltaProc = totalProcJiffies - prevProcJiffies;
        unsigned long long deltaTotal = currTotalJiffies - prevTotalJiffies;

        double cpuPct = 0.0;
        if (deltaTotal > 0)
            cpuPct = (double)deltaProc / deltaTotal * 100.0 * sysconf(_SC_NPROCESSORS_ONLN);

        prevProcTimes[pid] = totalProcJiffies;

        // Memory usage
        std::string key;
        long vmrss = 0, vmsize = 0;
        while (statusFile >> key) {
            if (key == "VmRSS:") statusFile >> vmrss;
            else if (key == "VmSize:") statusFile >> vmsize;
            statusFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        double memPct = 0.0;
        std::ifstream meminfo("/proc/meminfo");
        std::string mk;
        unsigned long memTotal = 0;
        while (meminfo >> mk) {
            if (mk == "MemTotal:") {
                meminfo >> memTotal;
                break;
            }
            meminfo.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if (memTotal)
            memPct = (vmrss / (double)memTotal) * 100.0;

        // Username
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        std::string username = pw ? pw->pw_name : "user";

        ProcessInfo p;
        p.pid = pid;
        p.user = username;
        p.cpu = cpuPct;
        p.mem = memPct;
        p.cmd = comm.substr(1, comm.size() - 2); // remove parentheses

        processes.push_back(p);
    }

    prevTotalJiffies = currTotalJiffies;

    // sort based on mode
    std::sort(processes.begin(), processes.end(), [sortMode](const ProcessInfo &a, const ProcessInfo &b) {
        if (sortMode == SortMode::CPU)
            return a.cpu > b.cpu;
        else
            return a.mem > b.mem;
    });


    return processes;
}

