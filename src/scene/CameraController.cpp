#include "scene/CameraController.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void CameraController::setEnabled(GLFWwindow* window, bool on) {
    enabled = on;
    glfwSetInputMode(window, GLFW_CURSOR,
                     on ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    if (on) m_firstMouse = true;  // prevent jump on re-enable
}

void CameraController::update(Camera& camera, GLFWwindow* window, float dt) {
    if (!enabled) return;

    // --- Mouse look ---
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (m_firstMouse) {
        // Initialise angles from the camera's current look direction
        glm::vec3 dir = glm::normalize(camera.target - camera.position);
        m_yaw   = glm::degrees(std::atan2(dir.z, dir.x));
        m_pitch = glm::degrees(std::asin(glm::clamp(dir.y, -1.f, 1.f)));
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    m_yaw   += sensitivity * static_cast<float>(xpos - m_lastX);
    m_pitch  += sensitivity * static_cast<float>(m_lastY - ypos);  // Y is inverted
    m_pitch  = glm::clamp(m_pitch, -89.f, 89.f);
    m_lastX  = xpos;
    m_lastY  = ypos;

    glm::vec3 forward;
    forward.x = std::cos(glm::radians(m_pitch)) * std::cos(glm::radians(m_yaw));
    forward.y = std::sin(glm::radians(m_pitch));
    forward.z = std::cos(glm::radians(m_pitch)) * std::sin(glm::radians(m_yaw));
    forward = glm::normalize(forward);

    camera.target = camera.position + forward;

    // --- WASD keyboard movement ---
    glm::vec3 right = glm::normalize(glm::cross(forward, camera.up));
    glm::vec3 move  = glm::vec3(0.f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) move += forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) move -= forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) move -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) move += right;

    if (glm::dot(move, move) > 0.f) {
        move = glm::normalize(move) * speed * dt;
        camera.position += move;
        camera.target   += move;
    }
}