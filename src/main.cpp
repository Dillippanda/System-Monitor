#include "cpu.h"
#include "memory.h"
#include "process.h"
#include "ui.h"

#include <thread>
#include <chrono>
#include <ncurses.h>
#include <csignal>
#include <sys/types.h>
#include <unistd.h>

int main() {
    CPU cpu;
    Memory mem;
    startUI();

    SortMode sortMode = SortMode::CPU;
    int selectedIndex = 0;
    int selectedPid = -1;
    std::vector<ProcessInfo> processes;

    double cpuUsage = 0.0;
    double memUsage = 0.0;

    MEVENT event;  // mouse event struct

    while (true) {
        cpuUsage = cpu.getUsage();
        memUsage = mem.getUsage();
        processes = getProcesses(sortMode);

        // Maintain highlight based on PID so it doesn’t jump
        if (selectedPid != -1) {
            for (int i = 0; i < (int)processes.size(); i++) {
                if (processes[i].pid == selectedPid) {
                    selectedIndex = i;
                    break;
                }
            }
        }

        render(cpuUsage, memUsage, processes, selectedIndex);

        int ch = getch();

        if (ch == 'q' || ch == 'Q') break;
        else if (ch == 'c' || ch == 'C') sortMode = SortMode::CPU;
        else if (ch == 'm' || ch == 'M') sortMode = SortMode::MEMORY;
        else if (ch == KEY_UP) {
            selectedIndex = std::max(0, selectedIndex - 1);
            if (!processes.empty())
                selectedPid = processes[selectedIndex].pid;
        }
        else if (ch == KEY_DOWN) {
            selectedIndex = std::min((int)processes.size() - 1, selectedIndex + 1);
            if (!processes.empty())
                selectedPid = processes[selectedIndex].pid;
        }
        else if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK && (event.bstate & BUTTON1_CLICKED)) {
                int rowClicked = event.y - 5; // process list starts at row 5
                if (rowClicked >= 0 && rowClicked < (int)processes.size()) {
                    selectedIndex = rowClicked;
                    selectedPid = processes[selectedIndex].pid;
                }
            }
        }
        else if (ch == 'k' || ch == 'K') {
            if (!processes.empty() && selectedIndex >= 0 && selectedIndex < (int)processes.size()) {
                pid_t pid = processes[selectedIndex].pid;
                selectedPid = pid;
                int result = kill(pid, SIGTERM);
                if (result == 0)
                    mvprintw(27, 0, "Process %d terminated successfully.", pid);
                else
                    mvprintw(27, 0, "Failed to terminate process %d (permission?).", pid);
                refresh();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // refresh rate
    }

    endUI();
    return 0;
}

