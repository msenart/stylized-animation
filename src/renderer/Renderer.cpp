#include "renderer/Renderer.h"
#include "core/Log.h"
#include <glad/glad.h>

Renderer::Renderer()
    : m_shader(Shader::fromFiles("blinn_phong.vert",
                                 "blinn_phong.frag")) {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::render(const Scene& scene, const AssetManager& assets, float aspect) {
    glClearColor(0.08f, 0.08f, 0.08f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.bind();
    m_shader.set("view",       scene.camera.view());
    m_shader.set("projection", scene.camera.projection(aspect));
    m_shader.set("viewPos",    scene.camera.position);

    if (!scene.lights.empty()) {
        const Light& l = scene.lights[0];
        m_shader.set("lightPos",   l.position);
        m_shader.set("lightColor", l.color * l.intensity);
    }

    m_drawCalls = 0;
    for (const Object& obj : scene.objects) {
        m_shader.set("model",       obj.transform.matrix());
        m_shader.set("objectColor", obj.material.color);
        assets.get(obj.meshHandle).draw();
        ++m_drawCalls;
    }
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}

void Renderer::reloadShaders() {
    try {
        m_shader = Shader::fromFiles("blinn_phong.vert",
                                     "blinn_phong.frag");
        Log::info("Shaders reloaded");
    } catch (const std::exception& e) {
        Log::error(std::string("Shader reload failed: ") + e.what());
    }
}