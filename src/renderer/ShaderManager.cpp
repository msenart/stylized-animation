#include "renderer/ShaderManager.h"
#include "renderer/Shader.h"
#include "core/Log.h"
#include <imgui.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------
// Internal types
// ---------------------------------------------------------------------------

namespace {

struct ShaderKey {
    std::string vert, frag, geom, tesc, tese;

    std::string getDescription() const {
        return "/"  + (vert.size() > 0 ? (vert + "/") : "")
                    + (geom.size() > 0 ? (geom + "/") : "")
                    + (tesc.size() > 0 ? (tesc + "/") : "")
                    + (tese.size() > 0 ? (tese + "/") : "")
                    + (frag.size() > 0 ? (frag + "/") : "");
    }

    bool operator==(const ShaderKey& o) const {
        return vert == o.vert && frag == o.frag && geom == o.geom
            && tesc == o.tesc && tese == o.tese;
    }
};

struct ShaderKeyHash {
    static void combine(std::size_t& seed, const std::string& s) {
        seed ^= std::hash<std::string>{}(s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    std::size_t operator()(const ShaderKey& k) const {
        std::size_t h = 0;
        combine(h, k.vert);
        combine(h, k.frag);
        combine(h, k.geom);
        combine(h, k.tesc);
        combine(h, k.tese);
        return h;
    }
};

struct ShaderEntry {
    ShaderKey              key;
    std::unique_ptr<Shader> shader;
    bool                   valid = false;
};

// ---------------------------------------------------------------------------
// Static storage
// ---------------------------------------------------------------------------

std::unordered_map<ShaderHandle, ShaderEntry>           g_entries;
std::unordered_map<ShaderKey, ShaderHandle, ShaderKeyHash> g_keyToHandle;
ShaderHandle                                            g_next = 1;

} // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

ShaderHandle ShaderManager::load(const std::string& vert, const std::string& frag,
                                  const std::string& geom, const std::string& tesc,
                                  const std::string& tese) {
    ShaderKey key{vert, frag, geom, tesc, tese};

    auto it = g_keyToHandle.find(key);
    if (it != g_keyToHandle.end())
        return it->second;

    ShaderHandle handle = g_next++;
    ShaderEntry& entry = g_entries[handle];
    entry.key    = key;
    entry.shader = std::make_unique<Shader>(Shader::fromFiles(vert, frag, geom, tesc, tese));
    entry.valid  = true;

    g_keyToHandle[key] = handle;
    Log::info("ShaderManager: loaded '" + entry.key.getDescription() +  "' -> handle " + std::to_string(handle));
    return handle;
}

const Shader& ShaderManager::get(ShaderHandle handle) {
    auto it = g_entries.find(handle);
    if (it == g_entries.end())
        throw std::runtime_error("ShaderManager::get - unknown handle " + std::to_string(handle));
    return *it->second.shader;
}

void ShaderManager::reloadAll() {
    int ok = 0, fail = 0;
    for (auto& [handle, entry] : g_entries) {
        const ShaderKey& k = entry.key;
        try {
            *entry.shader = Shader::fromFiles(k.vert, k.frag, k.geom, k.tesc, k.tese);
            entry.valid   = true;
            ++ok;
        } catch (const std::exception& e) {
            entry.valid = false;
            ++fail;
            Log::error("ShaderManager: reload failed for '" + k.vert + "': " + e.what());
        }
    }
    if (fail == 0)
        Log::info("ShaderManager: " + std::to_string(ok) + " shader(s) reloaded");
    else
        Log::warn("ShaderManager: " + std::to_string(ok) + " OK, " + std::to_string(fail) + " failed");
}

void ShaderManager::drawUI() {
    ImGui::Begin("Shaders");

    if (ImGui::Button("Reload All (F2)"))
        reloadAll();

    ImGui::Separator();
    ImGui::BeginTable("shaders_table", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg);
    ImGui::TableSetupColumn("Handle", ImGuiTableColumnFlags_WidthFixed, 50.f);
    ImGui::TableSetupColumn("Pipeline", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 55.f);
    ImGui::TableHeadersRow();

    for (const auto& [handle, entry] : g_entries) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%u", handle);
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted(entry.key.getDescription().c_str());
        ImGui::TableSetColumnIndex(2);
        if (entry.valid)
            ImGui::TextColored({0.4f, 1.f, 0.4f, 1.f}, "OK");
        else
            ImGui::TextColored({1.f, 0.3f, 0.3f, 1.f}, "ERROR");
    }

    ImGui::EndTable();
    ImGui::End();
}

void ShaderManager::shutdown() {
    g_entries.clear();
    g_keyToHandle.clear();
    g_next = 1;
}
