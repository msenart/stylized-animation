/**
 * @file Window.h
 * @brief RAII GLFW window and OpenGL 4.6 context.
 */
#pragma once
#include <string>

struct GLFWwindow; ///< Forward declaration to avoid pulling GLFW headers into every TU.

/**
 * @brief Creates and owns a GLFW window with an OpenGL 4.6 core profile context.
 *
 * GLAD function pointers are loaded during construction.
 * Non-copyable; GLFW is terminated in the destructor.
 */
class Window {
public:
    /**
     * @brief Initialises GLFW, creates the window, and loads OpenGL via GLAD.
     * @param width  Framebuffer width in pixels.
     * @param height Framebuffer height in pixels.
     * @param title  Window title string.
     * @throws std::runtime_error if GLFW or GLAD initialisation fails.
     */
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    /// @brief Returns true when the user has requested window closure.
    bool shouldClose() const;

    /// @brief Processes pending GLFW events (keyboard, mouse, resize…).
    void pollEvents() const;

    /// @brief Presents the back buffer to the screen.
    void swapBuffers() const;

    /// @brief Returns the native GLFW window handle.
    GLFWwindow* handle() const;

    /**
     * @brief Queries the current framebuffer dimensions.
     * @param[out] w Width  in pixels.
     * @param[out] h Height in pixels.
     */
    void getSize(int& w, int& h) const;

private:
    GLFWwindow* m_handle = nullptr;
};
