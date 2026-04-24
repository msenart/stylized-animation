/**
 * @file DebugConsole.h
 * @brief ImGui panel displaying the Log entry buffer with filters and search.
 */
#pragma once

/**
 * @brief An ImGui window that mirrors the Log entry buffer.
 *
 * Features: per-severity filter checkboxes (INFO / WARN / ERROR),
 * a substring search bar, a Clear button, and auto-scroll to the latest entry.
 */
class DebugConsole {
public:
    /**
     * @brief Renders the console window via ImGui.
     *
     * Must be called between ImGui::NewFrame() and ImGui::Render().
     */
    void draw();

private:
    bool m_showInfo  = true;
    bool m_showWarn  = true;
    bool m_showError = true;
    char m_search[128] = {};
};
