#include "core/Window.h"
#include "core/Log.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

Window::Window(int width, int height, const std::string& title) {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialise GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_handle);
    glfwSwapInterval(1); // vsync

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(m_handle);
        glfwTerminate();
        throw std::runtime_error("Failed to initialise GLAD");
    }

    Log::info("Window created: " + std::to_string(width) + "x" + std::to_string(height));
    Log::info(std::string("OpenGL ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Log::info(std::string("GPU: ")   + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
}

Window::~Window() {
    glfwDestroyWindow(m_handle);
    glfwTerminate();
}

bool        Window::shouldClose()  const { return glfwWindowShouldClose(m_handle); }
void        Window::pollEvents()   const { glfwPollEvents(); }
void        Window::swapBuffers()  const { glfwSwapBuffers(m_handle); }
GLFWwindow* Window::handle()       const { return m_handle; }

void Window::getSize(int& w, int& h) const {
    glfwGetFramebufferSize(m_handle, &w, &h);
}
