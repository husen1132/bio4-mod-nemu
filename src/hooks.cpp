#include "hooks.h"
#include <windows.h>
#include <d3d9.h>
#include <MinHook.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include "gui.h"

typedef HRESULT(APIENTRY* EndScene_t)(IDirect3DDevice9*);
static EndScene_t OriginalEndScene = nullptr;

typedef LRESULT(CALLBACK* WndProc_t)(HWND, UINT, WPARAM, LPARAM);
static WndProc_t OriginalWndProc = nullptr;

static HWND g_Window = nullptr;
static bool g_ImGuiInitialized = false;
static bool g_ShowMenu = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK HookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Read the F1 key toggle (hardcoded for now as reading INI is tricky without proper parsing)
    if (uMsg == WM_KEYDOWN && wParam == VK_F1) {
        g_ShowMenu = !g_ShowMenu;
        return 0; // block from game
    }

    if (g_ImGuiInitialized && g_ShowMenu) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
            return true;

        // Block mouse/keyboard input from game if menu is open
        if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP ||
            uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE || uMsg == WM_KEYDOWN || uMsg == WM_KEYUP ||
            uMsg == WM_CHAR) {
            return 0;
        }
    }

    return CallWindowProc(OriginalWndProc, hWnd, uMsg, wParam, lParam);
}

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
    DWORD wndProcId = 0;
    GetWindowThreadProcessId(handle, &wndProcId);

    if (GetCurrentProcessId() != wndProcId)
        return TRUE;

    // Check if window is visible and not a console
    char className[256];
    GetClassNameA(handle, className, sizeof(className));
    if (strcmp(className, "ConsoleWindowClass") == 0) return TRUE;

    g_Window = handle;
    return FALSE;
}

HWND GetGameWindow() {
    g_Window = nullptr;
    EnumWindows(EnumWindowsCallback, 0);
    return g_Window;
}

HRESULT APIENTRY HookedEndScene(IDirect3DDevice9* pDevice) {
    if (!g_ImGuiInitialized) {
        HWND window = GetGameWindow();
        if (window) {
            OriginalWndProc = (WndProc_t)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            // Note: INI file handling is disabled for now as it causes a linker error due to missing shell32 library in target_link_libraries.
            // When enabled, it allows saving window layout/positions between sessions.
            io.IniFilename = NULL; // Disable saving to imgui.ini

            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX9_Init(pDevice);

            GUI::Initialize();

            g_ImGuiInitialized = true;
        }
    }

    if (g_ImGuiInitialized) {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (g_ShowMenu) {
            GUI::Render();
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return OriginalEndScene(pDevice);
}

void InitializeHooks() {
    if (MH_Initialize() != MH_OK) {
        return;
    }

    // Use actual game window instead of dummy to ensure compatibility with RE4
    HWND hwnd = GetGameWindow();
    if (!hwnd) return;

    // Dummy device creation to get vtable
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D) return;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hwnd;

    IDirect3DDevice9* pDevice = nullptr;
    if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice))) {
        pD3D->Release();
        return;
    }

    void** pVTable = *reinterpret_cast<void***>(pDevice);

    // Hook EndScene (Index 42 in vtable)
    MH_CreateHook(pVTable[42], (void*)HookedEndScene, (void**)&OriginalEndScene);
    MH_EnableHook(MH_ALL_HOOKS);

    pDevice->Release();
    pD3D->Release();
}

void UninitializeHooks() {
    if (g_ImGuiInitialized) {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    if (OriginalWndProc && g_Window) {
        SetWindowLongPtr(g_Window, GWLP_WNDPROC, (LONG_PTR)OriginalWndProc);
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
