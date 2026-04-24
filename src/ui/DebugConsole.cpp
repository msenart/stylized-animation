#include "ui/DebugConsole.h"
#include "core/Log.h"
#include <imgui.h>

void DebugConsole::draw() {
    ImGui::Begin("Console");

    ImGui::Checkbox("INFO",  &m_showInfo);  ImGui::SameLine();
    ImGui::Checkbox("WARN",  &m_showWarn);  ImGui::SameLine();
    ImGui::Checkbox("ERROR", &m_showError); ImGui::SameLine();
    ImGui::SetNextItemWidth(160.f);
    ImGui::InputText("##search", m_search, sizeof(m_search)); ImGui::SameLine();
    ImGui::TextDisabled("Search"); ImGui::SameLine();
    if (ImGui::Button("Clear")) Log::entries.clear();

    ImGui::Separator();

    ImGui::BeginChild("##scrollregion", ImVec2(0, 0), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& e : Log::entries) {
        if (e.level == LogLevel::Info  && !m_showInfo)  continue;
        if (e.level == LogLevel::Warn  && !m_showWarn)  continue;
        if ((e.level == LogLevel::Error || e.level == LogLevel::GL) && !m_showError) continue;
        if (m_search[0] && e.message.find(m_search) == std::string::npos) continue;

        ImVec4 color {1.f, 1.f, 1.f, 1.f};
        if (e.level == LogLevel::Warn)                             color = {1.f, 1.f, 0.f, 1.f};
        if (e.level == LogLevel::Error || e.level == LogLevel::GL) color = {1.f, 0.3f, 0.3f, 1.f};

        ImGui::TextColored(color, "%s", e.message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.f);

    ImGui::EndChild();
    ImGui::End();
}
