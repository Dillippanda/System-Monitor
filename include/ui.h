#ifndef UI_H
#define UI_H

#include <vector>
#include "process.h"

void startUI();
void render(double cpuUsage, double memUsage, const std::vector<ProcessInfo> &processes, int selectedIndex);
void endUI();

#endif

