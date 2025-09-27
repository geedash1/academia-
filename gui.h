#pragma once
#include <d3d9.h>
#include <Windows.h>

namespace gui
{
    // Window size constants
    constexpr int WIDTH = 1920;
    constexpr int HEIGHT = 1080;

    // Window state
    inline bool isRunning = true;

    // WinAPI window
    inline HWND window = nullptr;
    inline WNDCLASSEX windowClass = { };
    inline POINTS position = { };

    // DirectX state
    inline PDIRECT3D9 d3d = nullptr;
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline D3DPRESENT_PARAMETERS presentParameters = { };

    // Functions for window
    void CreateHWindow(const char* windowName) noexcept;
    void DestroyHWindow() noexcept;

    // Functions for DirectX
    bool CreateDevice() noexcept;
    void ResetDevice() noexcept;
    void DestroyDevice() noexcept;

    // Functions for ImGui
    void CreateImGui() noexcept;
    void DestroyImGui() noexcept;
    void SetupImGuiStyle() noexcept;

    void BeginRender() noexcept;
    void EndRender() noexcept;
    void Render() noexcept;
}

