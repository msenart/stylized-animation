#include "ui/RenderStats.h"
#include "scene/Scene.h"
#include <imgui.h>

#include "core/MegaWindowContext.h"
#include "core/Window.h"
#include "GLFW/glfw3.h"
#include "scene/CameraController.h"

void RenderStats::draw(const Window& window, int drawCalls, std::size_t meshCount, float fps) {
    MegaWindowContext* window_context = static_cast<MegaWindowContext*>(glfwGetWindowUserPointer(window.handle()));
    ImGui::Begin("Render Stats");

    ImGui::Text("FPS        %.1f",    fps);
    ImGui::Text("Frame time %.2f ms", fps > 0.f ? 1000.f / fps : 0.f);

    ImGui::Separator();

    ImGui::Text("Draw calls %d",   drawCalls);
    ImGui::Text("Objects    %zu",  window_context->scene->objects.size());
    ImGui::Text("Lights     %zu",  window_context->scene->lights.size());
    ImGui::Text("Meshes     %zu",  meshCount);

    ImGui::Separator();

    const Camera& cam = window_context->scene->main_camera;
    ImGui::Text("Camera");
    ImGui::Text("  Position  (%.2f, %.2f, %.2f)",
                cam.position.x, cam.position.y, cam.position.z);
    ImGui::Text("  Target    (%.2f, %.2f, %.2f)",
                cam.target.x,   cam.target.y,   cam.target.z);
    ImGui::Text("  FOV       %.1f deg", cam.fov);

    CameraController* camCtrl = window_context->cameraController;
    ImGui::SliderFloat("Camera speed", &camCtrl->speed, 0.0f, 10.0f);
    ImGui::End();
}
