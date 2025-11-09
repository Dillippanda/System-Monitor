#include "ui.h"
#include <ncurses.h>
#include <iomanip>
#include <sstream>

void startUI() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // normal
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // mid CPU
    init_pair(3, COLOR_RED, COLOR_BLACK); // high CPU
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
}

void render(double cpuUsage, double memUsage, const std::vector<ProcessInfo> &processes, int selectedIndex) {
    clear();
    mvprintw(0, 0, "SYSTEM MONITOR TOOL (C++)");
    mvprintw(1, 0, "CPU Usage: %.2f%%    Memory Usage: %.2f%%", cpuUsage, memUsage);
    mvprintw(2, 0, "UP/DOWN Arrow navigate | Mouse click to select | C sort CPU | M sort MEM | K kill | Q quit");
    mvprintw(3, 0, "-------------------------------------------------------------");
    mvprintw(4, 0, "PID     USER           CPU%%   MEM%%   PROCESS");

    int row = 5;

    for (int i = 0; i < (int)processes.size() && i < 20; i++) {
        int color = 1;
        if (processes[i].cpu > 50) color = 3;
        else if (processes[i].cpu > 20) color = 2;

        if (i == selectedIndex)
            attron(A_REVERSE);

        attron(COLOR_PAIR(color));
        mvprintw(row++, 0, "%-7d %-8s %-6.1f %-6.1f %-s",
                 processes[i].pid,
                 processes[i].user.c_str(),
                 processes[i].cpu,
                 processes[i].mem,
                 processes[i].cmd.c_str());
        attroff(COLOR_PAIR(color));

        if (i == selectedIndex)
            attroff(A_REVERSE);
    }

    mvprintw(row + 1, 0, "UP/DOWN Arrow navigate | Mouse click to select | C sort CPU | M sort MEM | K kill | Q quit");
    refresh();
}




void endUI() {
    endwin();
}

