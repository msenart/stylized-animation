#include "renderer/ShaderManager.h"

#include "renderer/Shader.h"
#include "core/Log.h"
#include <imgui.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <set>

// ---------------------------------------------------------------------------
// Internal types
// ---------------------------------------------------------------------------

namespace {
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
        combine(h, k.comp);
        for (const auto& define : k.defines) {
            combine(h, define);
        }
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

std::unordered_map<ShaderHandle, ShaderEntry> g_entries;
std::unordered_map<ShaderKey, ShaderHandle, ShaderKeyHash> g_keyToHandle;
ShaderHandle g_next = 1;

} // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

ShaderHandle ShaderManager::load(std::set<std::string>& defines, const std::string& vert, const std::string& frag,
                                  const std::string& geom, const std::string& tesc,
                                  const std::string& tese) {
    ShaderKey key{vert, frag, geom, tesc, tese, defines};

    auto it = g_keyToHandle.find(key);
    if (it != g_keyToHandle.end())
        return it->second;

    ShaderHandle handle = g_next++;
    ShaderEntry entry = ShaderEntry{key, std::make_unique<Shader>(Shader::fromFiles(defines, vert, frag, geom, tesc, tese)), true};

    g_entries[handle] = std::move(entry);

    g_keyToHandle[key] = handle;
    Log::info("ShaderManager: loaded '" + entry.key.getDescription() +  "' -> handle " + std::to_string(handle));
    return handle;
}

ShaderHandle ShaderManager::load(ShaderKey& key) {
    auto it = g_keyToHandle.find(key);
    if (it != g_keyToHandle.end())
        return it->second;
    ShaderHandle handle = g_next++;
    std::unique_ptr<Shader> shader;
    if (key.isCompute())
        shader = std::make_unique<Shader>(Shader::computeFile(key.comp, key.defines));
    else
        shader = std::make_unique<Shader>(Shader::fromFiles(key.defines, key.vert, key.frag, key.geom, key.tesc, key.tese));
    ShaderEntry entry = ShaderEntry{key, std::move(shader), true};
    g_entries[handle] = std::move(entry);
    g_keyToHandle[key] = handle;
    Log::info("ShaderManager: loaded '" + key.getDescription() + "' -> handle " + std::to_string(handle));
    return handle;
}

ShaderHandle ShaderManager::loadCompute(ShaderKey& key) {
    auto it = g_keyToHandle.find(key);
    if (it != g_keyToHandle.end())
        return it->second;
    ShaderHandle handle = g_next++;
    ShaderEntry entry = ShaderEntry{key, std::make_unique<Shader>(Shader::computeFile(key.comp, key.defines)), true};
    g_entries[handle] = std::move(entry);
    g_keyToHandle[key] = handle;
    Log::info("ShaderManager: loaded compute '" + key.comp + "' -> handle " + std::to_string(handle));
    return handle;
}

ShaderHandle ShaderManager::getShaderHandleWithKey(const ShaderKey& shaderKey) {
    if (g_keyToHandle.count(shaderKey)) {
        return g_keyToHandle[shaderKey];
    }
    return 0;
}


const Shader& ShaderManager::get(ShaderHandle handle) {
    auto it = g_entries.find(handle);
    if (it == g_entries.end())
        throw std::runtime_error("ShaderManager::get - unknown handle " + std::to_string(handle));
    return *it->second.shader;
}

void ShaderManager::reloadAll()
{
    int ok = 0, fail = 0;

    for (auto& [handle, entry] : g_entries) {
        ShaderKey& key = entry.key;
        try {
            std::unique_ptr<Shader> newShader;
            if (key.isCompute())
                newShader = std::make_unique<Shader>(Shader::computeFile(key.comp, key.defines));
            else
                newShader = std::make_unique<Shader>(Shader::fromFiles(
                    key.defines, key.vert, key.frag, key.geom, key.tesc, key.tese));
            entry.shader = std::move(newShader);
            entry.valid  = true;
            ++ok;
        }
        catch (const std::exception& e) {
            ++fail;
            Log::error(
                "ShaderManager: reload failed for '" + (key.isCompute() ? key.comp : key.vert) + "'. "
                "Keeping previous shader. Error: " + e.what()
            );
        }
    }

    if (fail == 0)
        Log::info("ShaderManager: " + std::to_string(ok) + " shader(s) reloaded successfully.");
    else
        Log::warn("ShaderManager: " + std::to_string(ok) + " OK, "
                  + std::to_string(fail) + " failed — previous shader(s) preserved where available.");
}

void ShaderManager::drawUI() {
  if (!ImGui::Begin("Shaders")) {
        ImGui::End();
        return;
    }

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
