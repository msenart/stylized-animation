#include "renderer/ShaderManager.h"

#include <atomic>

#include "renderer/Shader.h"
#include "core/Log.h"
#include <imgui.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "core/MegaWindowContext.h"
#include <set>
#include "GLFW/glfw3.h"

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
        for (const auto& define : *k.defines) {
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

ShaderHandle ShaderManager::load(const std::string& vert, const std::string& frag,
                                  const std::string& geom, const std::string& tesc,
                                  const std::string& tese) {
    auto defines = std::make_shared<std::set<std::string>>();
    ShaderKey key{vert, frag, geom, tesc, tese, defines};

    auto it = g_keyToHandle.find(key);
    if (it != g_keyToHandle.end())
        return it->second;

    ShaderHandle handle = g_next++;
    ShaderEntry entry = ShaderEntry{key, std::make_unique<Shader>(Shader::fromFiles(vert, frag, geom, tesc, tese, defines)), true};

    g_entries[handle] = std::move(entry);

    g_keyToHandle[key] = handle;
    Log::info("ShaderManager: loaded '" + entry.key.getDescription() +  "' -> handle " + std::to_string(handle));
    return handle;
}

ShaderHandle ShaderManager::load(const ShaderKey& key) {
    auto it = g_keyToHandle.find(key);
    if (it != g_keyToHandle.end())
        return it->second;
    auto [vert, frag, geom, tesc, tese,defines] = key;
    ShaderHandle handle = g_next++;
    ShaderEntry entry = ShaderEntry{key, std::make_unique<Shader>(Shader::fromFiles(vert, frag, geom, tesc, tese, defines)), true};
    g_entries[handle] = std::move(entry);
    g_keyToHandle[key] = handle;
    Log::info("ShaderManager: loaded '" + entry.key.getDescription() +  "' -> handle " + std::to_string(handle));
    return handle;
}

ShaderHandle ShaderManager::getShaderHandleWithKey(const ShaderKey& shaderKey) {
    if (g_keyToHandle.count(shaderKey)) {
        return g_keyToHandle[shaderKey];
    }
    return 0;
}

static ShaderHandle acquireHandle(const ShaderKey& key)
{
    auto it = g_keyToHandle.find(key);
    if (it != g_keyToHandle.end())
        return it->second;

    ShaderHandle handle = g_next++;
    g_keyToHandle[key]  = handle;
    g_entries[handle].key = key;
    return handle;
}

static void releaseHandle(ShaderHandle handle)
{
    auto it = g_entries.find(handle);
    if (it == g_entries.end()) return;

    g_keyToHandle.erase(it->second.key);
    g_entries.erase(it);
}

const Shader& ShaderManager::get(ShaderHandle handle) {
    auto it = g_entries.find(handle);
    if (it == g_entries.end())
        throw std::runtime_error("ShaderManager::get - unknown handle " + std::to_string(handle));
    return *it->second.shader;
}

static std::atomic<ShaderHandle> g_nextHandle = 0;

void ShaderManager::reloadAll()
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        Log::error("ShaderManager: No active GLFW context.");
        return;
    }

    auto* ctx = static_cast<MegaWindowContext*>(glfwGetWindowUserPointer(window));
    if (!ctx || !ctx->scene) {
        Log::error("ShaderManager: Window context or Scene is missing.");
        return;
    }

    struct PendingAssignment {
        Object*      obj;
        PassTag      tag;
        ShaderHandle handle;
    };

    std::vector<PendingAssignment>   assignments;
    std::unordered_set<ShaderHandle> activeHandles;
    std::unordered_set<ShaderHandle> handlesToReload;

    for (auto& obj : ctx->scene->objects) {
        for (const auto& [tag, key] : obj.passTagShaderSpecifications) {
            ShaderHandle handle = acquireHandle(key);
            activeHandles.insert(handle);
            handlesToReload.insert(handle);
            assignments.push_back({ &obj, tag, handle });
        }
    }

    std::unordered_map<ShaderHandle, std::unique_ptr<Shader>> compiled;
    int ok = 0, fail = 0;

    for (ShaderHandle handle : handlesToReload) {
        const ShaderKey& key = g_entries[handle].key;
        try {
            Shader newShader = Shader::fromFiles(
                key.vert, key.frag, key.geom, key.tesc, key.tese, key.defines
            );
            compiled[handle] = std::make_unique<Shader>(std::move(newShader));
            ++ok;
        }
        catch (const std::exception& e) {
            ++fail;
            Log::error(
                "ShaderManager: reload failed for '" + key.vert + "'. "
                "Keeping previous shader. Error: " + e.what()
            );
        }
    }

    for (auto& [handle, newShader] : compiled) {
        auto& entry  = g_entries[handle];
        entry.shader = std::move(newShader);
        entry.valid  = true;
    }

    for (ShaderHandle handle : handlesToReload) {
        if (!compiled.count(handle) && !g_entries[handle].shader)
            g_entries[handle].valid = false;
    }

    for (const auto& a : assignments)
        a.obj->passTagShaderHandle[a.tag] = a.handle;

    for (auto it = g_entries.begin(); it != g_entries.end(); ) {
        if (!activeHandles.count(it->first)) {
            g_keyToHandle.erase(it->second.key); // ← sync g_keyToHandle
            it = g_entries.erase(it);
        } else {
            ++it;
        }
    }

    if (fail == 0)
        Log::info("ShaderManager: " + std::to_string(ok) + " shader(s) reloaded successfully.");
    else
        Log::warn("ShaderManager: " + std::to_string(ok) + " OK, "
                  + std::to_string(fail) + " failed — previous shader(s) preserved where available.");
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
