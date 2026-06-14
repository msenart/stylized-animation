#include "renderer/Renderer.h"
#include "renderer/ShaderManager.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include <glad/glad.h>
#include <map>
#include "renderer/SmearMesh.h"
#include "scene/Scene.h"
#include "RenderPipeline.h"

Renderer::Renderer() {
    glEnable(GL_DEPTH_TEST);

}

void Renderer::setup(Scene *scene, const Window *window, const AssetManager &assets, float aspect) {

    //create screen_mesh (rect that takes all the screen)
    glm::vec3 a = {1.0, 1.0, 0.0};
    std::vector<StaticVertex> vertices = {
        {{-1.0, -1.0, 0.0}, {0.0, 0.0, 1.0}}, //pos, normal
        {{-1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
        {{1.0, -1.0, 0.0}, {0.0, 0.0, 1.0}},
        {{1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
    };
    std::vector<uint32_t> indices = {0, 2, 1, 2, 3, 1}; //triangles
    p_screen_mesh = std::make_unique<StaticMesh>(vertices, indices); //instancie le mesh

    // Creating the rendering pipeline and the renderer
    m_renderPipeline = RenderPipeline();
    m_renderPipeline.addPass(std::make_shared<MeshIDRenderPass>());
    m_renderPipeline.addPass(std::make_shared<FinalRenderPass>());
    m_renderPipeline.addPass(std::make_shared<HybridRenderPass>());
    m_renderPipeline.addPass(std::make_shared<KAFRenderPass>());
    // Setting up the rendering pipeline and the renderer
    int w, h;
    window->getSize(w,h);
    m_renderPipeline.setup(w,h);
    // Setup the scene (especially insert the lights into the rendering pipeline)
    scene->setup();
}

void Renderer::render(Scene* scene,Window* window, const AssetManager& assets, float aspect, float dt) {
    RenderContext ctx{};
    ctx.camera = &scene->main_camera;
    ctx.scene = scene;
    ctx.window = window;

    m_drawCalls = 0;

    //time
    time = time + dt;
    while(time>max_time){
        time -= max_time;
    }


    // Hybrid pass
    m_renderPipeline.clear("Hybrid Render Pass");
    m_renderPipeline.execute("Hybrid Render Pass");
    for (unsigned int i = 0; i < scene->objects.size(); i++) {
        auto obj = scene->objects[i];
        auto it = obj.passTagShaderHandle.find(PassTag::Hybrid);
        if (it == obj.passTagShaderHandle.end()){
            Log::error("Did not find shader handle for Hybrid Render Pass for object whose meshHandle is "+std::to_string(obj.meshHandle));
            continue;
        }

        ShaderHandle handle = it->second;
        if (handle == 0) {
            Log::error("shader of obj "+std::to_string(obj.meshHandle)+" is null for hybrid render pass");
            continue;
        };

        const Mesh& mesh = assets.get(obj.meshHandle);

        const Shader& shader = ShaderManager::get(handle);
        shader.bind();
        shader.set("lightsNumber", static_cast<unsigned int>(scene->lights.size()));
        shader.set("model",obj.transform.matrix());
        shader.set("viewPos",scene->main_camera.position);
        shader.set("view",scene->main_camera.view());
        shader.set("projection",scene->main_camera.projection(aspect));
        // shader.set("objectBarycentre",glm::vec3(obj.transform.matrix()*glm::vec4(mesh.pseudo_barycentre(),1.0f)));
        shader.set("objectColor",obj.material.color);
        shader.set("meshId",static_cast<unsigned int>(i+1));
        mesh.uploadUniforms(shader, ctx);
        mesh.draw();
        ++m_drawCalls;
    }

    glDisable(GL_DEPTH_TEST);

    ShaderHandle debugHandle = ShaderManager::getShaderHandleWithKey(ShaderKey{"debug_bone.vert", "debug_bone.frag"});
    if (debugHandle != 0) {
      const Shader& debugShader = ShaderManager::get(debugHandle);
      debugShader.bind();
      debugShader.set("view", scene->main_camera.view());
      debugShader.set("projection", scene->main_camera.projection(aspect));

      for (unsigned int i = 0; i < scene->objects.size(); i++) {
        auto& obj = scene->objects[i];
        debugShader.set("model", obj.transform.matrix());
        debugShader.set("meshId", static_cast<unsigned int>(i+1));

        const Mesh& mesh = assets.get(obj.meshHandle);
        const SmearMesh* smearMesh = dynamic_cast<const SmearMesh*>(&mesh);
        if (smearMesh) {
          smearMesh->drawDebugBones(debugShader, ctx);
        }
      }
    }

    glEnable(GL_DEPTH_TEST);
    // Mesh ID pass
    // m_renderPipeline.clear("Mesh ID Render Pass");
    // m_renderPipeline.execute("Mesh ID Render Pass");
    // for (unsigned int i = 0; i < scene->objects.size(); i++) {
    //     auto obj = scene->objects[i];
    //     auto it = obj.passTagShaderHandle.find(PassTag::MeshID);
    //     if (it == obj.passTagShaderHandle.end()) {continue;}
    //     ShaderHandle handle = it->second;
    //     if (handle == 0) {continue;}

    // Screen pass
    //we draw a quad and fill it according to the shader
    //and the texture which comes from the hybrid render pass
    m_renderPipeline.clear("Final Render Pass");
    m_renderPipeline.execute("Final Render Pass");
    //find and bind shader
    ShaderHandle handle = std::static_pointer_cast<FinalRenderPass>(m_renderPipeline.getPass("Final Render Pass"))->shaderHandle();
    const Shader& shader = ShaderManager::get(handle);
    shader.bind();
    //now we bind the texture which comes from the previous framebuffer to the texture unit 0
    std::vector<unsigned int> hybridFboTexs = m_renderPipeline.getPassFboTexs("Hybrid Render Pass");
    if(hybridFboTexs.size() != 3){
        Log::error("hybrid render pass' fbo should have exactly 2 color attachments (textures), but it has "
            +std::to_string(hybridFboTexs.size())+" color attachments");
    }

    unsigned int texture = hybridFboTexs[0];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    texture = hybridFboTexs[1];
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    texture = hybridFboTexs[2];
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
    //glActiveTexture(GL_TEXTURE0);
    //we tell the shader that the texture named sceneTexture is in the texture unit 0
    //this texture is actually the one of the previous framebuffer
    shader.set("sceneTexture", 0);
    shader.set("metadataTexture", 1);
    shader.set("normalTexture", 2);
    int window_w, window_h;
    window->getSize(window_w, window_h);
    shader.set("window_w", window_w);//for final_contours.frag
    shader.set("window_h", window_h);//for final_contours.frag
    //others uniforms
    shader.set("time", time/max_time); //for final_perlin_background.frag
    //draw mesh
    p_screen_mesh->uploadUniforms(shader, ctx);
    p_screen_mesh->draw();

    ++m_drawCalls;
}

void Renderer::onResize(int width, int height) {
    m_renderPipeline.onResize(width, height);
}

int Renderer::drawCalls() const {
    return m_drawCalls;
}



// Mesh ID pass
// m_renderPipeline.clear("Mesh ID Render Pass");
// m_renderPipeline.execute("Mesh ID Render Pass");
// for (unsigned int i = 0; i < scene->objects.size(); i++) {
//     auto obj = scene->objects[i];
//     auto it = obj.passTagShaderHandle.find(PassTag::MeshID);
//     if (it == obj.passTagShaderHandle.end()) {continue;}
//     ShaderHandle handle = it->second;
//     if (handle == 0) {continue;}

//     const Mesh& mesh = assets.get(obj.meshHandle);

//     const Shader& shader = ShaderManager::get(handle);
//     shader.bind();
//     shader.set("model",obj.transform.matrix());
//     shader.set("view",scene->main_camera.view());
//     shader.set("projection",scene->main_camera.projection(aspect));
//     shader.set("meshID",static_cast<unsigned int>(i+1));
//     mesh.uploadUniforms(shader, ctx);
//     mesh.draw();
//     ++m_drawCalls;
// }

// old Screen pass
// m_renderPipeline.clear("Final Render Pass");
// m_renderPipeline.execute("Final Render Pass");
// for (const Object& obj : scene->objects) {
//     auto it = obj.passTagShaderHandle.find(PassTag::Final);
//     if (it == obj.passTagShaderHandle.end()) continue;

//     ShaderHandle handle = it->second;
//     if (handle == 0) continue;

//     const Mesh& mesh = assets.get(obj.meshHandle);

//     const Shader& shader = ShaderManager::get(handle);
//     shader.bind();
//     shader.set("model",obj.transform.matrix());
//     shader.set("viewPos",scene->main_camera.position);
//     shader.set("view",scene->main_camera.view());
//     shader.set("projection",scene->main_camera.projection(aspect));
//     shader.set("objectColor",obj.material.color);
//     mesh.uploadUniforms(shader, ctx);
//     mesh.draw();
//     ++m_drawCalls;
// }