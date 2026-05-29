#include "renderer/Renderer.h"
#include "renderer/ShaderManager.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include <glad/glad.h>
#include <map>
#include "scene/Scene.h"
#include "RenderPipeline.h"

Renderer::Renderer() {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::setup(Scene *scene, const Window *window, const AssetManager &assets, float aspect) {
    // Creating the rendering pipeline and the renderer
    m_renderPipeline = RenderPipeline();
    m_renderPipeline.addPass(std::make_shared<MeshIDRenderPass>());
    m_renderPipeline.addPass(std::make_shared<FinalRenderPass>());
    // Setting up the rendering pipeline and the renderer
    int w, h;
    window->getSize(w,h);
    m_renderPipeline.setup(w,h);
    // Setup the scene (especially insert the lights into the rendering pipeline)
    scene->setup();
}

void Renderer::render(Scene* scene,Window* window, const AssetManager& assets, float aspect) {
    RenderContext ctx{};
    ctx.camera = &scene->main_camera;
    ctx.scene = scene;
    ctx.window = window;

    m_drawCalls = 0;
    // Mesh ID pass
    m_renderPipeline.clear("Mesh ID Render Pass");
    m_renderPipeline.execute("Mesh ID Render Pass");
    for (unsigned int i = 0; i < scene->objects.size(); i++) {
        auto obj = scene->objects[i];
        auto it = obj.passTagShaderHandle.find(PassTag::MeshID);
        if (it == obj.passTagShaderHandle.end()) {continue;}
        ShaderHandle handle = it->second;
        if (handle == 0) {continue;}

        const Mesh& mesh = assets.get(obj.meshHandle);

        const Shader& shader = ShaderManager::get(handle);
        shader.bind();
        shader.set("model",obj.transform.matrix());
        shader.set("view",scene->main_camera.view());
        shader.set("projection",scene->main_camera.projection(aspect));
        shader.set("meshID",static_cast<unsigned int>(i+1));
        mesh.uploadUniforms(shader, ctx);
        mesh.draw();
        ++m_drawCalls;
    }

    // Screen pass
    m_renderPipeline.clear("Final Render Pass");
    m_renderPipeline.execute("Final Render Pass");
    for (const Object& obj : scene->objects) {
        auto it = obj.passTagShaderHandle.find(PassTag::Final);
        if (it == obj.passTagShaderHandle.end()) continue;

        ShaderHandle handle = it->second;
        if (handle == 0) continue;

        const Mesh& mesh = assets.get(obj.meshHandle);

        const Shader& shader = ShaderManager::get(handle);
        shader.bind();
        shader.set("model",obj.transform.matrix());
        shader.set("viewPos",scene->main_camera.position);
        shader.set("view",scene->main_camera.view());
        shader.set("projection",scene->main_camera.projection(aspect));
        shader.set("objectColor",obj.material.color);
        mesh.uploadUniforms(shader, ctx);
        mesh.draw();
        ++m_drawCalls;
    }

}

void Renderer::onResize(int width, int height) {
    m_renderPipeline.onResize(width, height);
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}
