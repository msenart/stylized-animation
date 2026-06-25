#pragma once
#include "RenderPipeline.h"
#include "scene/Scene.h"
#include "core/AssetManager.h"
#include "core/Window.h"
#include "renderer/Types_renderer.h"
#include "renderer/StaticMesh.h"






/**
 * @brief Issues OpenGL draw calls for every Object in a Scene.
 *
 * Each mesh selects its own shader; the renderer binds it and dispatches
 * uploadUniforms + draw per object.
 */
class Renderer {
public:
    Renderer();

    /// @brief Initialize the object members (essentially renderPipeline)
    void setup(Scene* scene, const Window* window, const AssetManager& assets, float aspect);

    /**
     * @brief Clears the framebuffer and draws every object in the scene.
     * @param scene Read-only scene providing camera, lights, and objects.
     * @param window Current rendering window
     * @param assets Read-only asset manager resolving MeshHandles.
     * @param aspect Viewport width / height for the projection matrix.
     * @param dt elapsed time since last frame
     */
    void render(Scene* scene, Window* window, const AssetManager& assets, float aspect, float dt);

    void onResize(int width, int height);

    RenderPipeline* renderPipeline() {
        return &m_renderPipeline;
    }

    /// @brief Returns the number of draw calls issued during the last render().
    int drawCalls() const;

    bool useKuwahara = false;

private:
    const float max_time = 2.0; //toutes les duex secondes
    float time=0; //time since the begining of the programm, modulo max_time
    std::unique_ptr<StaticMesh> p_screen_mesh; //used to render a rect on which to draw for the final render pass
    RenderPipeline m_renderPipeline;
    int m_drawCalls = 0;
};