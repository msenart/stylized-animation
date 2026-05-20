#include "Scene.h"

void Object::draw(bool *p_open) {
    if (!ImGui::Begin("Object Inspector", p_open)) {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 1.0f);
        ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.05f);
    }

    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Couleur", glm::value_ptr(material.color));
        ImGui::SliderFloat("Roughness", &material.roughness, 0.0f, 1.0f);
    }

    if (ImGui::CollapsingHeader("Shader Specifications")) {
        for (auto& [passTag, shaderKey] : passTagShaderSpecifications) {
            ImGui::PushID(PassTagToString(passTag).data());
            if (ImGui::TreeNode(("Pass Tag : " + PassTagToString(passTag)).c_str())) {

                ImGui::InputText("Vertex", shaderKey.vert.data(), 50);
                ImGui::InputText("Fragment", shaderKey.frag.data(),50);
                ImGui::InputText("Geometry", shaderKey.geom.data(),50);
                ImGui::InputText("Tess Control", shaderKey.tesc.data(),50);
                ImGui::InputText("Tess Eval", shaderKey.tese.data(),50);

                // -- Édition du std::set<std::string> (Defines) --
                ImGui::Separator();
                ImGui::Text("Defines :");

                std::string toRemove = "";
                for (const std::string& def : shaderKey.defines) {
                    ImGui::BulletText("%s", def.c_str());
                    ImGui::SameLine(ImGui::GetWindowWidth() - 50);

                    if (ImGui::Button(("X##" + def).c_str())) {
                        toRemove = def;
                    }
                }
                if (!toRemove.empty()) {
                    shaderKey.defines.erase(toRemove);
                }

                static char newDefineBuf[64] = "";
                ImGui::InputText("##NewDefine", newDefineBuf, IM_ARRAYSIZE(newDefineBuf));
                ImGui::SameLine();

                if (ImGui::Button("Add") && newDefineBuf[0] != '\0') {
                    shaderKey.defines.insert(std::string(newDefineBuf));
                    newDefineBuf[0] = '\0';
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    ImGui::End();
}
