/**
 * @file Renderer.h
 * @brief Forward rendering pass with a Blinn-Phong shader.
 */
#pragma once
#include "renderer/Shader.h"
#include "scene/Scene.h"
#include "core/AssetManager.h"

/**
 * @brief Issues OpenGL draw calls for every Object in a Scene.
 *
 * Owns a compiled shader program with embedded Blinn-Phong GLSL.
 * The Renderer reads from the Scene and AssetManager; it never mutates them.
 */
class Renderer {
public:
    /**
     * @brief Compiles the built-in shader and enables depth testing.
     * @throws std::runtime_error if shader compilation fails.
     */
    Renderer();

    /**
     * @brief Clears the framebuffer and draws every object in the scene.
     *
     * Uploads camera matrices and the first light's uniforms once per frame,
     * then iterates over all objects calling Mesh::draw().
     *
     * @param scene   Read-only scene providing camera, lights, and objects.
     * @param assets  Read-only asset manager resolving MeshHandles.
     * @param aspect  Viewport width / height for the projection matrix.
     */
    void render(const Scene& scene, const AssetManager& assets, float aspect);

    /// @brief Returns the number of draw calls issued during the last render().
    int drawCalls() const;

private:
    Shader m_shader;
    int    m_drawCalls = 0;
};
