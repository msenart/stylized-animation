/**
 * @file CameraController.h
 * @brief Keyboard + mouse first-person camera controller.
 */
#pragma once
#include "scene/Camera.h"

struct GLFWwindow;

/**
 * @brief First-person camera controller driven by WASD keys and mouse look.
 *
 * Call setEnabled() when toggling camera capture (F1), and update() once per frame.
 * While enabled the cursor is hidden and locked; it is restored on disable.
 */
class CameraController {
public:
    float speed       = 5.f;   ///< Movement speed in world units per second.
    float sensitivity = 0.1f;  ///< Mouse sensitivity in degrees per pixel.
    bool  enabled     = false;

    /**
     * @brief Enables or disables camera control.
     *
     * Locks/unlocks the cursor and resets first-mouse state to prevent
     * a position jump when re-enabling.
     */
    void setEnabled(GLFWwindow* window, bool on);

    /**
     * @brief Polls mouse delta and WASD keys; updates camera position and target.
     * @param camera  Camera to move.
     * @param window  GLFW window to poll input from.
     * @param dt      Delta time in seconds.
     */
    void update(Camera& camera, GLFWwindow* window, float dt);

private:
    float  m_yaw        = -90.f;
    float  m_pitch      = 0.f;
    double m_lastX      = 0.0;
    double m_lastY      = 0.0;
    bool   m_firstMouse = true;
};