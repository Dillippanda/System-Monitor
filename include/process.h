#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>

enum class SortMode {
    CPU,
    MEMORY
};

struct ProcessInfo {
    int pid;
    std::string user;
    double cpu;
    double mem;
    std::string cmd;
};

std::vector<ProcessInfo> getProcesses(SortMode sortMode = SortMode::CPU);

#endif

