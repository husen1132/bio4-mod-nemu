# AI Agent Instructions for RE4 Mod Menu

## Scope
These instructions apply to the entire `re4_mod_menu` project.

## Architecture
- **dinput8.dll**: The mod acts as a proxy DLL for `DirectInput8Create`. The game loads this DLL instead of the system one, allowing us to inject code.
- **MinHook**: Used to hook DirectX 9 (`EndScene`) and `WndProc` to intercept rendering and input for the GUI.
- **ImGui**: Used for the graphical user interface.
- **Game Version**: Resident Evil 4 UHD is a 32-bit game, so all compilation MUST target 32-bit (`i686-w64-mingw32`).

## Guidelines
- Do not modify the build toolchain (`toolchain-mingw32.cmake`) unless absolutely necessary.
- When adding new features, add a new tab in `src/gui.cpp` or integrate it logically into an existing tab.
- Stub out memory reading/writing (like in `src/egg_spawner.cpp`) with clear comments unless specific offsets are provided.
