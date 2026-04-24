#include "ui/RenderStats.h"
#include "scene/Scene.h"
#include <imgui.h>

void RenderStats::draw(const Scene& scene, int drawCalls, std::size_t meshCount, float fps) {
    ImGui::Begin("Render Stats");

    ImGui::Text("FPS        %.1f",    fps);
    ImGui::Text("Frame time %.2f ms", fps > 0.f ? 1000.f / fps : 0.f);

    ImGui::Separator();

    ImGui::Text("Draw calls %d",   drawCalls);
    ImGui::Text("Objects    %zu",  scene.objects.size());
    ImGui::Text("Lights     %zu",  scene.lights.size());
    ImGui::Text("Meshes     %zu",  meshCount);

    ImGui::Separator();

    const Camera& cam = scene.camera;
    ImGui::Text("Camera");
    ImGui::Text("  Position  (%.2f, %.2f, %.2f)",
                cam.position.x, cam.position.y, cam.position.z);
    ImGui::Text("  Target    (%.2f, %.2f, %.2f)",
                cam.target.x,   cam.target.y,   cam.target.z);
    ImGui::Text("  FOV       %.1f deg", cam.fov);

    ImGui::End();
}
