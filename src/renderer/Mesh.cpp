#include "renderer/Mesh.h"
#include "renderer/Shader.h"

void Mesh::uploadUniforms(const Shader& shader, const RenderContext& ctx) const {
    shader.set("model",      ctx.model);
    shader.set("view",       ctx.view);
    shader.set("projection", ctx.projection);
}