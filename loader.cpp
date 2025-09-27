#include "gui.h"
#include <string>
#include "../imgui/imgui.h"

namespace loader
{
    static bool loggedIn = false;
    static char username[64] = "";
    static char password[64] = "";
    static bool showPassword = false;

    void Render()
    {
        if (!loggedIn)
        {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);
            ImGui::Begin("Academia++ Loader", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Login").x) * 0.5f);
            ImGui::Text("Login");
            ImGui::Separator();

            ImGui::InputText("Username", username, IM_ARRAYSIZE(username));
            ImGui::InputText("Password", password, IM_ARRAYSIZE(password),
                showPassword ? 0 : ImGuiInputTextFlags_Password);

            ImGui::SameLine();
            ImGui::Checkbox("Show", &showPassword);

            ImGui::Spacing();

            if (ImGui::Button("Login", ImVec2(380, 0)))
            {
                
                if (std::string(username) == "test" && std::string(password) == "1234")
                {
                    loggedIn = true;
                }
                else
                {
                    ImGui::OpenPopup("Login Failed");
                }
            }

            if (ImGui::BeginPopupModal("Login Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Invalid username or password.");
                if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::End();
        }
        else
        {
            
            gui::Render();
        }
    }
}

