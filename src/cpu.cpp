#include "cpu.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

CPU::CPU() : prevIdle(0), prevTotal(0) {}

void CPU::readCPU(unsigned long long &idle, unsigned long long &total) {
    std::ifstream file("/proc/stat");
    std::string line;
    getline(file, line);
    std::istringstream ss(line);

    std::string cpu;
    unsigned long long user, nice, system, idle_t, iowait, irq, softirq, steal;
    ss >> cpu >> user >> nice >> system >> idle_t >> iowait >> irq >> softirq >> steal;

    idle = idle_t + iowait;
    total = user + nice + system + idle_t + iowait + irq + softirq + steal;
}

double CPU::getUsage() {
    unsigned long long idle, total;
    readCPU(idle, total);

    unsigned long long diffIdle = idle - prevIdle;
    unsigned long long diffTotal = total - prevTotal;

    double cpuPercent = 0.0;
    if (diffTotal > 0)
        cpuPercent = (1.0 - ((double)diffIdle / diffTotal)) * 100.0;

    prevIdle = idle;
    prevTotal = total;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return cpuPercent;
}

