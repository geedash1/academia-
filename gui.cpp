#include "gui.h"
#include "login.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include "../resource.h"
#include <string>
#include <thread>
#include <chrono>

namespace features
{
    inline bool autoPetTrainer = false;   // Auto Pet Trainer
    inline bool enableAutoFarm = false;   // Auto Farm
    inline int selectedDungeon = 0;       // Dungeon selector
    inline bool autoQuest = false;        // Auto Quest
    inline float speedMultiplier = 1.0f;  // For increasers
    inline float critPercent = 0.0f;
    inline float damagePercent = 0.0f;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

long __stdcall WindowProcess(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_SIZE:
        if (gui::device && wParam != SIZE_MINIMIZED)
        {
            gui::presentParameters.BackBufferWidth = LOWORD(lParam);
            gui::presentParameters.BackBufferHeight = HIWORD(lParam);
            gui::ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_LBUTTONDOWN:
        gui::position = MAKEPOINTS(lParam);
        return 0;
    case WM_MOUSEMOVE:
        if (wParam == MK_LBUTTON)
        {
            const auto pts = MAKEPOINTS(lParam);
            RECT rect{};
            GetWindowRect(gui::window, &rect);
            rect.left += pts.x - gui::position.x;
            rect.top += pts.y - gui::position.y;
            if (gui::position.x >= 0 && gui::position.x <= gui::WIDTH &&
                gui::position.y >= 0 && gui::position.y <= 19)
                SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
        }
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

// ---------------- Window ----------------
void gui::CreateHWindow(const char* windowName) noexcept
{
    HINSTANCE hInstance = GetModuleHandleA(nullptr);
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = WindowProcess;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    windowClass.hCursor = 0;
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = "class001";
    windowClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    RegisterClassEx(&windowClass);

    window = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        "class001",
        windowName,
        WS_POPUP,
        100, 100, WIDTH, HEIGHT,
        0, 0,
        windowClass.hInstance, 0
    );

    SetLayeredWindowAttributes(window, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

// ---------------- DirectX ----------------
bool gui::CreateDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));
    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0)
        return false;

    return true;
}

void gui::ResetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    if (device->Reset(&presentParameters) == D3DERR_INVALIDCALL) IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
    if (device) { device->Release(); device = nullptr; }
    if (d3d) { d3d->Release(); d3d = nullptr; }
}

// ---------------- ImGui ----------------
void gui::SetupImGuiStyle() noexcept
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.1f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.05f, 0.15f, 0.95f);
    colors[ImGuiCol_Border] = ImVec4(0.5f, 0.3f, 0.9f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.6f, 0.4f, 0.9f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.9f, 0.6f, 1.0f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.7f, 1.0f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.9f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.7f, 1.0f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void gui::CreateImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);
    SetupImGuiStyle();
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// ---------------- Rendering ----------------
void gui::BeginRender() noexcept
{
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) { isRunning = false; return; }
    }
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
    ImGui::EndFrame();
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }
    if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST &&
        device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

// ---------------- GUI ----------------
void gui::Render() noexcept
{
    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("Academia++", &isRunning, ImGuiWindowFlags_NoCollapse);

    static int sidebar = 0;
    const char* categories[] = { "Spoofing", "Scripts", "Auto Farm", "Increasers" };

    ImGui::BeginChild("Sidebar", ImVec2(150, 0), true);
    for (int i = 0; i < IM_ARRAYSIZE(categories); i++)
        if (ImGui::Selectable(categories[i], sidebar == i))
            sidebar = i;
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("MainContent", ImVec2(0, 0), false);

    if (sidebar == 0)
    {
        if (ImGui::Button("Spoof Membership", ImVec2(-1, 0)))
        {
            // TODO: implement membership spoofing logic
        }
    }
    else if (sidebar == 1)
    {
        ImGui::Checkbox("Auto Pet Trainer", &features::autoPetTrainer);
        ImGui::Checkbox("Auto Quest", &features::autoQuest);
    }
    else if (sidebar == 2)
    {
        ImGui::Checkbox("Enable Auto Farm", &features::enableAutoFarm);
        if (features::enableAutoFarm)
        {
            const char* dungeons[] =
            { "Mount Olympus", "WaterWorks", "Mirror Lake",
              "Darkmoor Part 1", "Darkmoor Part 2", "Darkmoor Part 3" };
            ImGui::Text("Select dungeon to farm:");
            ImGui::Combo("##DungeonSelector", &features::selectedDungeon, dungeons, IM_ARRAYSIZE(dungeons));
            ImGui::Separator();
            ImGui::Text("Status: %s", dungeons[features::selectedDungeon]);
        }
    }
    if (sidebar == 3) // Increasers
    {
        ImGui::Text("Increasers Options");

        // Static variables to store slider values
        static float speedMultiplier = 1.0f;     // 1x = normal speed
        static float critPercent = 0.0f;         // 0% to 100%
        static float damagePercent = 0.0f;       // 0% to 200%

        // Sliders
        ImGui::SliderFloat("Speed Multiplier", &speedMultiplier, 1.0f, 5.0f, "%.1fx");
        ImGui::SliderFloat("Crit %", &critPercent, 0.0f, 100.0f, "%.0f%%");
        ImGui::SliderFloat("Damage %", &damagePercent, 0.0f, 200.0f, "%.0f%%");

        ImGui::Spacing();
        ImGui::Text("Current Values:");
        ImGui::Text("Speed: %.1fx", speedMultiplier);
        ImGui::Text("Crit: %.0f%%", critPercent);
        ImGui::Text("Damage: %.0f%%", damagePercent);

        // Optional: connect these values to your features/game logic
        // features::speedMultiplier = speedMultiplier;
        // features::critPercent = critPercent;
        // features::damagePercent = damagePercent;
    }

    ImGui::EndChild();
    ImGui::End();
}
