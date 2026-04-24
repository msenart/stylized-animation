/**
 * @file RenderStats.h
 * @brief ImGui panel displaying frame timing and scene statistics.
 */
#pragma once
#include <cstddef>

struct Scene;

/**
 * @brief An ImGui window showing FPS, draw calls, scene counts, and camera state.
 */
class RenderStats {
public:
    /**
     * @brief Renders the stats window via ImGui.
     *
     * Must be called between ImGui::NewFrame() and ImGui::Render().
     *
     * @param scene      Read-only scene providing camera and object counts.
     * @param drawCalls  Number of draw calls issued in the last frame.
     * @param meshCount  Total number of meshes in the AssetManager.
     * @param fps        Frames per second (1 / deltaTime).
     */
    void draw(const Scene& scene, int drawCalls, std::size_t meshCount, float fps);
};
