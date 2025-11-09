#ifndef CPU_H
#define CPU_H

#include <string>

class CPU {
public:
    CPU();
    double getUsage();  // returns CPU percentage between two reads
private:
    unsigned long long prevIdle, prevTotal;
    void readCPU(unsigned long long &idle, unsigned long long &total);
};

#endif

