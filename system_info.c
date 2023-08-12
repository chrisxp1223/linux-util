#include <ncurses.h>
#include <stdlib.h>
#include <pci/pci.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <stdio.h>

// Function declarations
void show_system_info();
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <ncurses.h>
#include <pci/pci.h>

void show_pci_space();
void show_pci_registers(struct pci_dev *device);

int main(void) {
    // Initialize the screen
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    // Create a new window for the menu
    WINDOW *menu_win = newwin(1, COLS, 0, 0);
    keypad(menu_win, TRUE);

    // Menu items
    char *menu_items[] = {
        "System Info",
        "PCI SPACE",
    };

    int menu_size = sizeof(menu_items) / sizeof(menu_items[0]);
    int selected = 0;

    // Show the System Info window by default
    show_system_info();

    // Main loop
    while (1) {
        // Draw menu
        for (int i = 0; i < menu_size; i++) {
            if (i == selected) {
                wattron(menu_win, A_REVERSE);
            }
            mvwprintw(menu_win, 0, i * 12, "%-12s", menu_items[i]);
            wattroff(menu_win, A_REVERSE);
        }
        wrefresh(menu_win);

        // Handle input
        int key = wgetch(menu_win);
        switch (key) {
            case KEY_LEFT:
                selected = (selected - 1 + menu_size) % menu_size;
                break;
            case KEY_RIGHT:
                selected = (selected + 1) % menu_size;
                break;
            case 10: // Enter key
                if (selected == 0) {
                    show_system_info();
                } else if (selected == 1) {
                    show_pci_space();
                }
                break;
            case 'q':
                endwin();
                return 0;
        }
    }
}

void show_system_info() {
    clear();
    refresh();

    WINDOW *info_win = newwin(LINES - 2, COLS - 2, 1, 1);
    box(info_win, 0, 0);

    // Get system information
    struct sysinfo sys_info;
    sysinfo(&sys_info);
    unsigned long total_ram = sys_info.totalram / 1024 / 1024;
    unsigned long free_ram = sys_info.freeram / 1024 / 1024;

    // Get uname information (includes CPU info)
    struct utsname uname_info;
    uname(&uname_info);

    mvwprintw(info_win, 1, 1, "System Information:");
    mvwprintw(info_win, 3, 1, "Processor: %s", uname_info.machine);
    mvwprintw(info_win, 4, 1, "CPU Name: %s %s", uname_info.sysname, uname_info.release);
    mvwprintw(info_win, 5, 1, "Memory: Total: %lu MB, Free: %lu MB", total_ram, free_ram);
    wrefresh(info_win);

    wgetch(info_win);

    delwin(info_win);
    refresh();
}

void show_pci_space() {
    clear();
    refresh();

    WINDOW *pci_win = newwin(LINES - 2, COLS - 2, 1, 1);
    box(pci_win, 0, 0);

    mvwprintw(pci_win, 1, 1, "PCI Space:");

    struct pci_access *pacc;
    struct pci_dev *dev;
    char device_name[256];

    pacc = pci_alloc();
    pci_init(pacc);
    pci_scan_bus(pacc);

    int row = 3;
    int selected = 0;
    int device_count = 0;

    for (dev = pacc->devices; dev; dev = dev->next) {
        device_count++;
    }

    keypad(pci_win, TRUE);

    struct pci_dev *selected_dev = NULL;
    
    while (1) {
        row = 3;
        int current = 0;

        for (dev = pacc->devices; dev; dev = dev->next) {
            pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);
            pci_lookup_name(pacc, device_name, sizeof(device_name), PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);

            if (current == selected) {
                wattron(pci_win, A_REVERSE);
            }

            mvwprintw(pci_win, row, 1, "Bus: %02x, Device: %02x, Vendor: %04x, Device ID: %04x, Name: %s",
                      dev->bus, dev->dev, dev->vendor_id, dev->device_id, device_name);

            wattroff(pci_win, A_REVERSE);

            row++;
            current++;
        }

        wrefresh(pci_win);

        // Handle input
        int key = wgetch(pci_win);
        switch (key) {
            case KEY_UP:
                selected = (selected - 1 + device_count) % device_count;
                break;
            case KEY_DOWN:
                selected = (selected + 1) % device_count;
                break;
            case 10: // Enter key
                // Find the selected device
                selected_dev = pacc->devices;
                for (int i = 0; i < selected; i++) {
                    selected_dev = selected_dev->next;
                }
                // Show PCI registers for the selected device
                show_pci_registers(selected_dev);
                break;
            case 'q':
                delwin(pci_win);
                refresh();
                return;
        }
    }

    pci_cleanup(pacc);
}

void show_pci_registers(struct pci_dev *device) {
    clear();
    refresh();

    WINDOW *registers_win = newwin(LINES - 2, COLS - 2, 1, 1);
    box(registers_win, 0, 0);
    keypad(registers_win, TRUE);

    mvwprintw(registers_win, 1, 1, "PCI Registers (Bus: %02x, Device: %02x):", device->bus, device->dev);

    int row = 3;
    for (int i = 0; i < 256; i += 16) {
        u8 buffer[16];
        pci_read_block(device, i, buffer, sizeof(buffer));
        mvwprintw(registers_win, row, 1, "%02x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                  i, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
                  buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
        row++;
    }
    
    wrefresh(registers_win);

    wgetch(registers_win);

    delwin(registers_win);
    
    refresh();
}