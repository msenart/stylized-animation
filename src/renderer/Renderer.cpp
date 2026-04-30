#include "renderer/Renderer.h"
#include "renderer/ShaderManager.h"
#include <glad/glad.h>

#include "Shader.h"

Renderer::Renderer()
    : m_shaderHandle(ShaderManager::load("blinn_phong.vert", "blinn_phong.frag")) {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::render(const Scene& scene, const AssetManager& assets, float aspect) {
    glClearColor(0.08f, 0.08f, 0.08f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Shader& sh = ShaderManager::get(m_shaderHandle);
    sh.bind();
    sh.set("view",       scene.camera.view());
    sh.set("projection", scene.camera.projection(aspect));
    sh.set("viewPos",    scene.camera.position);

    if (!scene.lights.empty()) {
        const Light& l = scene.lights[0];
        sh.set("lightPos",   l.position);
        sh.set("lightColor", l.color * l.intensity);
    }

    m_drawCalls = 0;
    for (const Object& obj : scene.objects) {
        sh.set("model",       obj.transform.matrix());
        sh.set("objectColor", obj.material.color);
        assets.get(obj.meshHandle).draw();
        ++m_drawCalls;
    }
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}
