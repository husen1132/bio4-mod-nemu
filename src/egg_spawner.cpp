#include "egg_spawner.h"
#include "imgui.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace EggSpawner {

    struct EntityInfo {
        std::string name;
        int id; // Placeholder for internal game ID
    };

    std::vector<EntityInfo> entities = {
        {"Ganado (Villager)", 1},
        {"Ganado (Zealot)", 2},
        {"Ganado (Soldier)", 3},
        {"Dr. Salvador", 4},
        {"Bella Sisters", 5},
        {"Garrador", 6},
        {"Regenerador", 7},
        {"Iron Maiden", 8},
        {"Colmillo (Dog)", 9},
        {"Novistador", 10},
        {"El Gigante", 11}
        // Excludes bosses like Salazar as requested
    };

    bool openInventoryAfter = false;
    bool infiniteEgg = false;
    char searchBuffer[256] = "";
    int selectedEntityIdx = -1;

    // Simple case-insensitive search
    bool StringContainsFuzzy(const std::string& str, const std::string& query) {
        if (query.empty()) return true;

        std::string strLower = str;
        std::string queryLower = query;

        std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
        std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);

        // Very basic "fuzzy": just checking if the query is a substring.
        // Real fuzzy search (like Levenshtein distance) would go here if needed.
        return strLower.find(queryLower) != std::string::npos;
    }

    void Render(bool* open) {
        ImGui::Begin("Egg Spawner", open);

        ImGui::InputText("Search Entity", searchBuffer, IM_ARRAYSIZE(searchBuffer));

        ImGui::BeginChild("EntityList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true);

        for (int i = 0; i < entities.size(); ++i) {
            if (StringContainsFuzzy(entities[i].name, searchBuffer)) {
                if (ImGui::Selectable(entities[i].name.c_str(), selectedEntityIdx == i)) {
                    selectedEntityIdx = i;
                }
            }
        }

        ImGui::EndChild();
        ImGui::Separator();

        // Bottom Bar
        ImGui::Checkbox("Open Inventory Manager After Add Egg", &openInventoryAfter);
        ImGui::SameLine();
        ImGui::Checkbox("Infinite Egg?", &infiniteEgg);
        ImGui::SameLine();

        ImGui::BeginDisabled(selectedEntityIdx == -1);
        if (ImGui::Button("Add egg")) {
            // Memory writing logic would go here based on the selected entity ID
            //
            // e.g.
            // uintptr_t inventoryBase = *reinterpret_cast<uintptr_t*>(0x12345678);
            // AddItemToInventory(inventoryBase, EGG_ITEM_ID, entities[selectedEntityIdx].id, infiniteEgg);
            //
            // if (openInventoryAfter) {
            //     ForceOpenInventoryMenu();
            // }

            // For now just stubbed.
        }
        ImGui::EndDisabled();

        ImGui::End();
    }
}
