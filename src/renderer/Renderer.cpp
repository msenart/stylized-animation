#include "renderer/Renderer.h"
#include "renderer/ShaderManager.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include <glad/glad.h>

Renderer::Renderer() {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::render(const Scene& scene, const AssetManager& assets, float aspect) {
    glClearColor(0.08f, 0.08f, 0.08f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderContext ctx;
    ctx.view = scene.camera.view();
    ctx.projection = scene.camera.projection(aspect);
    ctx.viewPos = scene.camera.position;

    if (!scene.lights.empty()) {
        const Light& l = scene.lights[0];
        ctx.lightPos = l.position;
        ctx.lightColor = l.color * l.intensity;
    }

    m_drawCalls = 0;
    for (const Object& obj : scene.objects) {
        ctx.model = obj.transform.matrix();
        ctx.objectColor = obj.material.color;

        const Mesh& mesh = assets.get(obj.meshHandle);
        const Shader& shader = ShaderManager::get(mesh.shaderHandle);
        shader.bind();
        mesh.uploadUniforms(shader, ctx);
        mesh.draw();
        ++m_drawCalls;
    }
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}