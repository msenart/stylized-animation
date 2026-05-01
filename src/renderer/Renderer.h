#pragma once
#include "scene/Scene.h"
#include "core/AssetManager.h"

/**
 * @brief Issues OpenGL draw calls for every Object in a Scene.
 *
 * Each mesh selects its own shader; the renderer binds it and dispatches
 * uploadUniforms + draw per object.
 */
class Renderer {
public:
    Renderer();

    /**
     * @brief Clears the framebuffer and draws every object in the scene.
     * @param scene   Read-only scene providing camera, lights, and objects.
     * @param assets  Read-only asset manager resolving MeshHandles.
     * @param aspect  Viewport width / height for the projection matrix.
     */
    void render(const Scene& scene, const AssetManager& assets, float aspect);

    /// @brief Returns the number of draw calls issued during the last render().
    int drawCalls() const;

private:
    int m_drawCalls = 0;
};