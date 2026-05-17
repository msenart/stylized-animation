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

void Renderer::render(Scene* scene,Window* window, const AssetManager& assets, float aspect) {
    glClearColor(0.08f, 0.08f, 0.08f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderContext ctx;
    ctx.camera = &scene->main_camera;
    ctx.scene = scene;
    ctx.window = window;

    m_drawCalls = 0;
    // Mesh ID pass
    // Screen pass
    for (const Object& obj : scene->objects) {
        auto it = obj.passTagShaderHandle.find(PassTag::Renderable);
        if (it == obj.passTagShaderHandle.end()) continue;

        ShaderHandle handle = it->second;
        if (handle == 0) continue;

        const Mesh& mesh = assets.get(obj.meshHandle);

        const Shader& shader = ShaderManager::get(handle);
        shader.bind();
        shader.set("model",obj.transform.matrix());
        shader.set("view",scene->main_camera.view());
        shader.set("projection",scene->main_camera.projection(aspect));
        shader.set("objectColor",obj.material.color);
        mesh.uploadUniforms(shader, ctx);
        mesh.draw();
        ++m_drawCalls;
    }
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}