#pragma once
#include "renderer/ShaderManager.h"
#include "scene/Scene.h"
#include "core/AssetManager.h"

/**
 * @brief Issues OpenGL draw calls for every Object in a Scene.
 *
 * Registers its shader with ShaderManager on construction. Use
 * ShaderManager::reloadAll() to hot-reload shaders from disk.
 */
class Renderer {
public:
    /**
     * @brief Registers the Blinn-Phong shader and enables depth testing.
     * @throws std::runtime_error if shader compilation fails.
     */
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
    ShaderHandle m_shaderHandle;
    int          m_drawCalls = 0;
};
