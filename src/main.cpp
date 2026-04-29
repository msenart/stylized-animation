#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/Log.h"
#include "core/Window.h"
#include "core/AssetManager.h"
#include "scene/Scene.h"
#include "scene/CameraController.h"
#include "renderer/Geometry.h"
#include "renderer/Renderer.h"
#include "ui/DebugConsole.h"
#include "ui/RenderStats.h"

static void setupGLDebugCallback() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum, GLenum type, GLuint, GLenum severity,
                              GLsizei, const GLchar* message, const void*) {
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
        if (type == GL_DEBUG_TYPE_ERROR)
            Log::error(std::string("GL: ") + message);
        else
            Log::warn(std::string("GL: ") + message);
    }, nullptr);
}

int main() {
    Window window(1280, 720, "Stylized Animation Engine");
    setupGLDebugCallback();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.handle(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    AssetManager assets;

    auto cubeData = Geometry::makeCube();
    MeshHandle cubeHandle = assets.add(
        std::make_unique<Mesh>(cubeData.vertices, cubeData.indices));

    auto fileMeshData = Geometry::loadMeshFromFile( "assets/meshes/stanford-bunny.obj");
    // Log::info("how many meshes loaded from the scene ? " + std::to_string(fileMeshData.size()));
    MeshHandle fileMeshHandle = assets.add(std::make_unique<Mesh>(fileMeshData[0].vertices, fileMeshData[0].indices));

    Scene scene;

    Object obj;
    obj.meshHandle     = fileMeshHandle;
    obj.material.color = {0.8f, 0.3f, 0.2f};
    scene.objects.push_back(obj);
    obj.transform.scale = glm::vec3(100.0f);

    Light light;

    light.position  = {3.f, 5.f, 3.f};
    light.intensity = 1.f;
    scene.lights.push_back(light);

    scene.camera.position = {0.f, 1.f, 5.f};
    scene.camera.target   = {0.f, 0.f, 0.f};

    Renderer         renderer;
    CameraController camCtrl;
    DebugConsole     console;
    RenderStats      stats;

    Log::info("Engine ready");
    Log::info("F1 -> toggle camera control | F2 -> reload shaders");

    double prevTime = glfwGetTime();
    float  fps      = 0.f;

    // Edge-trigger state for toggle keys
    bool prevF1 = false;
    bool prevF2 = false;

    while (!window.shouldClose()) {
        window.pollEvents();

        double now = glfwGetTime();
        float  dt  = static_cast<float>(now - prevTime);
        prevTime   = now;
        fps        = dt > 0.f ? 1.f / dt : 0.f;

        // F1 — toggle camera control
        bool f1 = glfwGetKey(window.handle(), GLFW_KEY_F1) == GLFW_PRESS;
        if (f1 && !prevF1) {
            camCtrl.setEnabled(window.handle(), !camCtrl.enabled);
            Log::info(camCtrl.enabled ? "Camera control ON" : "Camera control OFF");
        }
        prevF1 = f1;

        // F2 — hot-reload shaders from disk
        bool f2 = glfwGetKey(window.handle(), GLFW_KEY_F2) == GLFW_PRESS;
        if (f2 && !prevF2)
            renderer.reloadShaders();
        prevF2 = f2;

        // ZQSD camera movement
        camCtrl.update(scene.camera, window.handle(), dt);

        int w, h;
        window.getSize(w, h);
        glViewport(0, 0, w, h);

        float aspect = (h > 0) ? static_cast<float>(w) / static_cast<float>(h) : 1.f;
        renderer.render(scene, assets, aspect);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        console.draw();
        stats.draw(scene, renderer.drawCalls(), assets.meshCount(), fps);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}