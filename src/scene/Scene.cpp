#include "Scene.h"

#include <filesystem>

#include "glad/glad.h"

static int inputTextResizeCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto* str = static_cast<std::string*>(data->UserData);
        str->resize(data->BufTextLen);
        data->Buf     = str->data();
        data->BufSize = static_cast<int>(str->capacity()) + 1;
    }
    return 0;
}

static bool InputTextString(const char* label, std::string& str,
                            ImGuiInputTextFlags flags = 0)
{
    if (str.capacity() < str.size() + 128)
        str.reserve(str.size() + 128);

    return ImGui::InputText(
        label,
        str.data(),
        str.capacity() + 1,
        flags | ImGuiInputTextFlags_CallbackResize,
        inputTextResizeCallback,
        static_cast<void*>(&str)
    );
}


namespace {

    bool shaderFileExists(const std::string& relativePath)
    {
        if (relativePath.empty()) return true;
        return std::filesystem::exists(SHADER_FOLDER_PATH + relativePath);
    }

    static bool ShaderPathInput(const char* label, std::string& path, bool optional = false)
    {
        static std::string backup;
        static std::string activeLabel;

        const bool empty   = path.empty();
        const bool exists  = shaderFileExists(path);
        const bool valid   = (optional && empty) || exists; // vide = OK seulement si optionnel
        const bool problem = !valid;

        ImVec4 dotColor = (optional && empty) ? ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)
                        : exists              ? ImVec4(0.2f, 0.85f, 0.2f, 1.f)
                                              : ImVec4(0.95f, 0.2f, 0.2f, 1.f);
        ImGui::TextColored(dotColor, "●");
        ImGui::SameLine();

        if (problem) {
            ImGui::PushStyleColor(ImGuiCol_FrameBg,        ImVec4(0.4f, 0.05f, 0.05f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.5f, 0.10f, 0.10f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  ImVec4(0.6f, 0.15f, 0.15f, 1.f));
        }

        const bool confirmed = InputTextString(label, path, ImGuiInputTextFlags_EnterReturnsTrue);
        const bool isFocused = ImGui::IsItemFocused();

        if (problem) ImGui::PopStyleColor(3);

        // Prise de focus → sauvegarde
        if (isFocused && activeLabel != label) {
            activeLabel = label;
            backup      = path;
        }

        // Rollback si la nouvelle valeur n'est pas acceptable
        auto shouldRollback = [&]() {
            if (!optional && path.empty()) return true; // obligatoire et vide → rollback
            if (!path.empty() && !exists)  return true; // renseigné mais introuvable → rollback
            return false;
        };

        if (activeLabel == label) {
            if (confirmed) {
                if (shouldRollback()) path = backup;
                else                  backup = path;
                activeLabel = "";
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                path        = backup;
                activeLabel = "";
            }
            else if (!isFocused) {
                if (shouldRollback()) path = backup;
                activeLabel = "";
            }
        }

        if (problem && ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1.f, 0.4f, 0.4f, 1.f), "File not found:");
            ImGui::Text("%s", (SHADER_FOLDER_PATH + path).c_str());
            ImGui::EndTooltip();
        }

        if (optional && !empty) {
            ImGui::SameLine();
            ImGui::PushID("clear");
            if (ImGui::SmallButton("x")) { path.clear(); backup.clear(); activeLabel = ""; }
            ImGui::PopID();
        }

        return confirmed && valid;
    }


} // namespace>

static void ShaderDefinesDisplay(const std::set<std::string>& defines)
{
    if (defines.empty()) {
        ImGui::TextDisabled("No defines.");
        return;
    }

    for (const auto& define : defines) {
        ImGui::TextColored(ImVec4(0.6f, 0.85f, 1.f, 1.f), "#define");
        ImGui::SameLine();
        ImGui::Text("%s", define.c_str());
    }
}

void Object::draw(bool *p_open) {
    if (!ImGui::Begin("Object Inspector", p_open)) {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 1.0f);
        ImGui::DragFloat3("Scale",    glm::value_ptr(transform.scale),    0.05f);
    }

    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Couleur",   glm::value_ptr(material.color));
        ImGui::SliderFloat("Roughness", &material.roughness, 0.0f, 1.0f);
    }

    if (ImGui::CollapsingHeader("Shader Specifications")) {
        for (auto& [passTag, shaderKey] : passTagShaderSpecifications) {
            ImGui::PushID(PassTagToString(passTag).data());

            if (ImGui::TreeNode(("Pass Tag : " + PassTagToString(passTag)).c_str())) {

                bool allValid;
                if (shaderKey.isCompute()) {
                    ShaderPathInput("Compute", shaderKey.comp);
                    allValid = shaderFileExists(shaderKey.comp);
                } else {
                    ShaderPathInput("Vertex",       shaderKey.vert);
                    ShaderPathInput("Fragment",     shaderKey.frag);
                    ShaderPathInput("Geometry",     shaderKey.geom, /*optional=*/true);
                    ShaderPathInput("Tess Control", shaderKey.tesc, /*optional=*/true);
                    ShaderPathInput("Tess Eval",    shaderKey.tese, /*optional=*/true);
                    allValid = shaderFileExists(shaderKey.vert)
                            && shaderFileExists(shaderKey.frag)
                            && shaderFileExists(shaderKey.geom)
                            && shaderFileExists(shaderKey.tesc)
                            && shaderFileExists(shaderKey.tese);
                }

                ImGui::Separator();
                ImGui::BeginDisabled(!allValid);
                if (ImGui::Button("Reload Shader"))
                    ShaderManager::reloadAll();
                ImGui::EndDisabled();

                if (!allValid) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.f, 0.5f, 0.1f, 1.f),
                                       "Fix missing files before reloading.");
                }

                ImGui::Separator();
                ImGui::Text("Defines");
                ShaderDefinesDisplay(shaderKey.defines);

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    ImGui::End();
}

void Scene::setup() {
    glCreateBuffers(1, &lights_ssbo);
    glNamedBufferData(lights_ssbo, lights.size() * sizeof(Light), lights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lights_ssbo);
}
