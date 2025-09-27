#include "gui.h"
#include "login.h"
#include <Windows.h>
#include <thread>
#include <chrono>

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
 
    if (!ShowLoginWindow()) 
    {
        MessageBoxA(nullptr, "Login failed or cancelled.", "Academia++", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    \
    gui::isRunning = true;
    gui::window = nullptr;
    gui::d3d = nullptr;
    gui::device = nullptr;

    gui::CreateHWindow("Academia++");
    if (!gui::CreateDevice())
    {
        MessageBoxA(nullptr, "Failed to create D3D device for main GUI.", "Error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }

    gui::CreateImGui();

    
    while (gui::isRunning)
    {
        gui::BeginRender();
        gui::Render();
        gui::EndRender();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return EXIT_SUCCESS;
}


