#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "common.h"

// ! the order decides which module get processed first
Module* modules[MODULE_CNT] = {
    &lagModule,
    &dropModule,
    &throttleModule,
    &dupModule,
    &oodModule,
    &tamperModule,
    &resetModule,
	&bandwidthModule,
};

volatile short sendState = SEND_STATUS_NONE;

// Hardcoded WinDivert filter for UDP ports 5000-8000
#define RIVEN_FILTER "udp and ((udp.DstPort >= 5000 and udp.DstPort <= 8000) or (udp.SrcPort >= 5000 and udp.SrcPort <= 8000))"

int main(int argc, char* argv[]) {
    char buf[MSG_BUFSIZE];

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    // Hide the console window so Riven runs invisibly
    {
        HWND hConsole = GetConsoleWindow();
        if (hConsole != NULL) {
            ShowWindow(hConsole, SW_HIDE);
        }
    }

    LOG("Riven starting (headless mode)");
    LOG("Is Run As Admin: %d", IsRunAsAdmin());
    LOG("Is Elevated: %d", IsElevated());

    // Initialize random seed
    srand((unsigned int)time(NULL));

    // Start the WinDivert packet capture with hardcoded filter
    if (divertStart(RIVEN_FILTER, buf) == 0) {
        LOG("Failed to start divert: %s", buf);
        return 1;
    }

    LOG("Riven started successfully. Filter: %s", RIVEN_FILTER);

    // Sleep forever - the divert threads handle everything.
    // Process can only be terminated via Task Manager.
    Sleep(INFINITE);

    return 0;
}
