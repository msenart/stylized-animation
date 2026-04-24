#include "renderer/Renderer.h"
#include <glad/glad.h>

static const char* k_vert = R"glsl(
    #version 460 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 fragPos;
    out vec3 normal;

    void main() {
        fragPos     = vec3(model * vec4(aPos, 1.0));
        normal      = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * vec4(fragPos, 1.0);
    }
)glsl";

static const char* k_frag = R"glsl(
    #version 460 core
    in vec3 fragPos;
    in vec3 normal;

    uniform vec3 lightPos;
    uniform vec3 lightColor;
    uniform vec3 viewPos;
    uniform vec3 objectColor;

    out vec4 fragColor;

    void main() {
        vec3 norm     = normalize(normal);
        vec3 lightDir = normalize(lightPos - fragPos);
        vec3 viewDir  = normalize(viewPos  - fragPos);
        vec3 halfDir  = normalize(lightDir + viewDir);

        float ambient = 0.15;
        float diff    = max(dot(norm, lightDir), 0.0);
        float spec    = pow(max(dot(norm, halfDir), 0.0), 32.0);

        vec3 result = (ambient + diff + 0.5 * spec) * lightColor * objectColor;
        fragColor   = vec4(result, 1.0);
    }
)glsl";

Renderer::Renderer() : m_shader(k_vert, k_frag) {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::render(const Scene& scene, const AssetManager& assets, float aspect) {
    glClearColor(0.08f, 0.08f, 0.08f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.bind();
    m_shader.setMat4("view",       scene.camera.view());
    m_shader.setMat4("projection", scene.camera.projection(aspect));
    m_shader.setVec3("viewPos",    scene.camera.position);

    if (!scene.lights.empty()) {
        const Light& l = scene.lights[0];
        m_shader.setVec3("lightPos",   l.position);
        m_shader.setVec3("lightColor", l.color * l.intensity);
    }

    m_drawCalls = 0;
    for (const Object& obj : scene.objects) {
        m_shader.setMat4("model",       obj.transform.matrix());
        m_shader.setVec3("objectColor", obj.material.color);
        assets.get(obj.meshHandle).draw();
        ++m_drawCalls;
    }
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}
