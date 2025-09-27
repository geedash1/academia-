#include "login.h"
#include "gui.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <Windows.h>
#include "../imgui/imgui_internal.h"

static void TextCentered(const char* text)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (!window) { ImGui::Text("%s", text); return; }
    const float windowWidth = ImGui::GetWindowSize().x;
    const float textWidth = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text);
}

bool ShowLoginWindow()
{
    gui::isRunning = true;

    gui::CreateHWindow("Academia++ - Loader");
    if (!gui::CreateDevice())
    {
        OutputDebugStringA("CreateDevice failed\n");
        gui::DestroyHWindow();
        return false;
    }
    gui::CreateImGui();

    bool loginSuccess = false;
    static char username[64] = "";
    static char password[64] = "";

    std::vector<std::string> stages = {
        "Resolving symbols",
        "Preparing environment",
        "Resolving dependencies",
        "Patching memory",
        "Injecting payload",
        "Finalizing"
    };

    size_t currentStage = 0;
    float stageProgress = 0.0f;
    const float stageDurationSeconds = 0.7f;
    int dotCounter = 0;
    float dotTimer = 0.0f;
    const float dotInterval = 0.35f;

    auto lastTime = std::chrono::steady_clock::now();

    while (gui::isRunning && !loginSuccess)
    {
        gui::BeginRender();

        ImGui::SetNextWindowSize(ImVec2(460, 280), ImGuiCond_FirstUseEver);
        ImGui::Begin("Login", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

        ImGui::Spacing();
        TextCentered("Academia++ Loader");
        ImGui::Separator();
        ImGui::Spacing();

        if (currentStage == 0)
        {
            ImGui::InputText("Username", username, IM_ARRAYSIZE(username));
            ImGui::InputText("Password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);

            if (ImGui::Button("Login", ImVec2(-1, 0)))
            {
                if (std::string(username) == "G-" && std::string(password) == "password")
                {
                    currentStage = 1; // start loader stages
                    lastTime = std::chrono::steady_clock::now();
                }
                else
                {
                    ImGui::OpenPopup("Login Failed");
                }
            }

            if (ImGui::BeginPopupModal("Login Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Invalid username or password.");
                if (ImGui::Button("OK", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
        }
        else
        {
            // loader animation
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - lastTime).count();
            lastTime = now;

            stageProgress += dt / stageDurationSeconds;
            if (stageProgress >= 1.0f)
            {
                stageProgress = 0.0f;
                if (currentStage < stages.size() - 1)
                    currentStage++;
                else
                    loginSuccess = true;
            }

            float overall = static_cast<float>(currentStage) / stages.size() + stageProgress / stages.size();
            if (overall > 1.0f) overall = 1.0f;

            dotTimer += dt;
            if (dotTimer >= dotInterval) { dotTimer = 0.0f; dotCounter = (dotCounter + 1) % 4; }

            std::string dots(dotCounter, '.');
            ImGui::Text("%s%s", stages[currentStage].c_str(), dots.c_str());
            ImGui::ProgressBar(overall, ImVec2(-1.0f, 0.0f));
        }

        ImGui::End();
        gui::EndRender();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // do NOT destroy device/window here — main GUI will reuse them
    gui::isRunning = true;

    return true;
}