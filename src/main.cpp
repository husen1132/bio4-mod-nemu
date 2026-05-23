#include <windows.h>
#include "hooks.h"
#include "dinput8.h"

DWORD WINAPI MainThread(LPVOID lpReserved) {
    while (!GetGameWindow()) {
        Sleep(100);
    }
    // Give the game a bit of time to fully initialize after window creation
    Sleep(500);
    InitializeHooks();
    return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
            break;
        case DLL_PROCESS_DETACH:
            UninitializeHooks();
            break;
    }
    return TRUE;
}
