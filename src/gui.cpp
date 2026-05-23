#include "gui.h"
#include "imgui.h"
#include "egg_spawner.h"

namespace GUI {
    bool showEggSpawner = false;

    void Initialize() {
        // Initialize any GUI specific state here
    }

    void Render() {
        ImGui::Begin("RE4 Mod Menu", nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::BeginChild("Tabs", ImVec2(150, 0), true);

        static int selectedTab = 0;

        if (ImGui::Selectable("Main", selectedTab == 0)) selectedTab = 0;
        if (ImGui::Selectable("Misc", selectedTab == 1)) selectedTab = 1;
        // Add more tabs as needed

        ImGui::EndChild();
        ImGui::SameLine();

        ImGui::BeginChild("Content", ImVec2(0, 0), true);

        if (selectedTab == 0) {
            ImGui::Text("Main Settings");
            ImGui::Separator();

            if (ImGui::Button("Open egg spawner")) {
                showEggSpawner = true;
            }

            // Add other main settings here
        } else if (selectedTab == 1) {
            ImGui::Text("Misc Settings");
            ImGui::Separator();
            // Add misc settings here
        }

        ImGui::EndChild();
        ImGui::End();

        if (showEggSpawner) {
            EggSpawner::Render(&showEggSpawner);
        }
    }
}
