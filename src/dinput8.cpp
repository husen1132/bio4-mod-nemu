#include "dinput8.h"

typedef HRESULT(WINAPI* DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
static DirectInput8Create_t OriginalDirectInput8Create = nullptr;

void InitDInput8() {
    char syspath[MAX_PATH];
    GetSystemDirectoryA(syspath, MAX_PATH);
    strcat_s(syspath, "\\dinput8.dll");

    HMODULE hMod = LoadLibraryA(syspath);
    if (hMod) {
        OriginalDirectInput8Create = (DirectInput8Create_t)GetProcAddress(hMod, "DirectInput8Create");
    }
}

extern "C" HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
    if (!OriginalDirectInput8Create) {
        InitDInput8();
    }

    if (OriginalDirectInput8Create) {
        return OriginalDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }

    return E_FAIL;
}
