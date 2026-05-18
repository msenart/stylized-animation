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
    m_render_pipeline = RenderPipeline();
    m_render_pipeline.addPass(std::make_shared<MeshIDRenderPass>());
    m_render_pipeline.addPass(std::make_shared<FinalRenderPass>());
    // Setting up the rendering pipeline and the renderer
    int w, h;
    window->getSize(w,h);
    m_render_pipeline.setup(w,h);
}

void Renderer::render(Scene* scene,Window* window, const AssetManager& assets, float aspect) {
    RenderContext ctx{};
    ctx.camera = &scene->main_camera;
    ctx.scene = scene;
    ctx.window = window;

    m_drawCalls = 0;
    // Mesh ID pass
    m_render_pipeline.clear("MeshIDRenderPass");
    m_render_pipeline.execute("MeshIDRenderPass");
    for (unsigned int i = 0; i < scene->objects.size(); i++) {
        auto obj = scene->objects[i];
        auto it = obj.passTagShaderHandle.find(PassTag::MeshIDRenderPass);
        if (it == obj.passTagShaderHandle.end()) {continue;}
        ShaderHandle handle = it->second;
        if (handle == 0) {continue;}

        const Mesh& mesh = assets.get(obj.meshHandle);

        const Shader& shader = ShaderManager::get(handle);
        shader.bind();
        shader.set("model",obj.transform.matrix());
        shader.set("view",scene->main_camera.view());
        shader.set("projection",scene->main_camera.projection(aspect));
        shader.set("meshID",static_cast<unsigned int>(1));
        mesh.uploadUniforms(shader, ctx);
        mesh.draw();
        ++m_drawCalls;
    }

    // Screen pass
    m_render_pipeline.clear("FinalRenderPass");
    m_render_pipeline.execute("FinalRenderPass");
    for (const Object& obj : scene->objects) {
        auto it = obj.passTagShaderHandle.find(PassTag::FinalRenderPass);
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
    m_render_pipeline.onResize(width, height);
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}
