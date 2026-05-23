//
// Created by mathi on 16/05/2026.
//
#include "SelectionManager.h"
#include "core/MegaWindowContext.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "scene/Scene.h"

void SelectionManager::select(glm::vec2 mousePos, unsigned int window_h) {
    auto fbo = m_render_pipeline->getPassFbo("Mesh ID Render Pass");

    if (fbo == 0) {
        m_selection = nullptr;
        return;
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    int x = static_cast<int>(mousePos.x);
    int y = static_cast<int>(mousePos.y);

    GLuint pixelId = 0;

    glReadPixels(x, window_h - y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixelId);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    if (pixelId == 0) {
        m_selection = nullptr;
    } else {
        size_t objectIndex = static_cast<size_t>(pixelId - 1);

        auto& objects = m_scene->objects;

        if (objectIndex < objects.size()) {
            m_selection = &objects[objectIndex];
        } else {
            m_selection = nullptr;
        }
    }
}

void SelectionManager::draw() {
    if (m_selection != nullptr) {
        m_selection->draw();
    }
}
