#include "memory.h"
#include <fstream>
#include <string>
#include <limits>

double Memory::getUsage() {
    std::ifstream file("/proc/meminfo");
    std::string key;
    unsigned long total = 0, free = 0, available = 0;

    while (file >> key) {
        if (key == "MemTotal:") file >> total;
        else if (key == "MemAvailable:") file >> available;
        if (total && available) break;
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    if (total == 0) return 0.0;
    double used = total - available;
    return (used / total) * 100.0;
}

