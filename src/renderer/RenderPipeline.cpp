//
// Created by mathi on 16/05/2026.
//

#include "RenderPipeline.h"

#include "core/Log.h"
#include "glad/glad.h"

// render passes

// HybridRenderPass implementation 

void HybridRenderPass::setup(unsigned int window_w, unsigned int window_h) {
    // creating frame buffer and bind it
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // creating texture 1 (scene)
    glGenTextures(1, &m_fboTex1);
    glBindTexture(GL_TEXTURE_2D, m_fboTex1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_w, window_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //GL_COLOR_ATTACHMENT0 : 1er emplacement pour la texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTex1, 0);

    // creating texture 2 (metadata : meshId, contour, etc...)
    glGenTextures(1, &m_fboTex2);
    glBindTexture(GL_TEXTURE_2D, m_fboTex2);
    //will use GL_R32UI later, for now use GL_RGBA because it is easier AND GL_RED_INTEGER instead of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, window_w, window_h, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //GL_COLOR_ATTACHMENT1 : 2ème emplacement pour la texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_fboTex2, 0);

    
    // creating texture 3 (for normals -> used for contours detection)
    glGenTextures(1, &m_fboTex3);
    glBindTexture(GL_TEXTURE_2D, m_fboTex3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_w, window_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); //test GL_FLOAT ?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //GL_COLOR_ATTACHMENT2 : 3ème emplacement pour la texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_fboTex3, 0);

    //create a depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_w, window_h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // attach the 3 color attachment to the framebuffer
    GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    //why ???
    glBindTexture(GL_TEXTURE_2D, 0);

    //check if everything is ok
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        Log::error("Framebuffer "+ name() +"  could not be created : " + std::to_string(fboStatus));
    }

    //switch to default framebuffer (id=0)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HybridRenderPass::execute() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); //enable face culling
}

void HybridRenderPass::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    

    // clear all the buffer of the color attachments specified by glDrawBuffer (see setup)
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //used to clear the buffer of 1 omponent
    GLuint uclearValue[4] = {0, 0, 0, 0};
    glClearBufferuiv(GL_COLOR, 1, uclearValue); //second arg : drawBuffer -> refers to color attachment

    //clear the normal buffer 
    GLfloat clearValue[4] = {0.0, 0.0, 0.0, 0.0};
    glClearBufferfv(GL_COLOR, 2, clearValue);
}

// MeshIDRenderPass implementation

void MeshIDRenderPass::setup(unsigned int window_w, unsigned int window_h) {
    // creating frame buffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // creating texture for the frame buffer
    glGenTextures(1, &m_fboTex);
    glBindTexture(GL_TEXTURE_2D, m_fboTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, window_w, window_h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTex, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        Log::error("Framebuffer "+ name() +"  could not be created : " + std::to_string(fboStatus));
    }

    //switch to default framebuffer (id=0)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MeshIDRenderPass::execute() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glEnable(GL_DEPTH_TEST);
}

void MeshIDRenderPass::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    GLuint clearValue = 0;
    glClearBufferuiv(GL_COLOR, 0, &clearValue);
}

void MeshIDRenderPass::onResize(unsigned int window_w, unsigned int window_h) {
    // glDeleteTextures(1, &m_fbo);
    setup(window_w, window_h);
}

// final render pass

void FinalRenderPass::setup(unsigned window_w, unsigned window_h) {}

void FinalRenderPass::execute() {
    //switch to default framebuffer (id=0)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void FinalRenderPass::clear() {
    //switch to default framebuffer (id=0)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FinalRenderPass::onResize(unsigned int window_w, unsigned int window_h) {}

// render pipeline

void RenderPipeline::addPass(const std::shared_ptr<RenderPass> &pass) {
    std::string name = pass->name();

    if (mapRenderPasses.count(name) > 0) {
        Log::warn("The pass '" + name + "' has the same name as another pass already in the render pipeline! Overwriting...");
    }

    //fill a map to get a pointer to the pass from its string name
    mapRenderPasses[name] = pass;
}

void RenderPipeline::setup(unsigned int window_w, unsigned int window_h) const {
    for (auto& pair : mapRenderPasses) {
        pair.second->setup(window_w,window_h);
    }
}

void RenderPipeline::execute(const std::string& name) {
    auto it = mapRenderPasses.find(name);
    if (it != mapRenderPasses.end()) {
        it->second->execute();
    } else {
        Log::error("Failed to execute render pass: '" + name + "' not found.");
    }
}

void RenderPipeline::clear(const std::string& name) {
    auto it = mapRenderPasses.find(name);
    if (it != mapRenderPasses.end()) {
        it->second->clear();
    } else {
        Log::error("Failed to clear render pass: '" + name + "' not found.");
    }
}

void RenderPipeline::onResize(unsigned int window_w, unsigned int window_h) const {
    for (auto& pair : mapRenderPasses) {
        pair.second->onResize(window_w, window_h);
    }
}

unsigned int RenderPipeline::getPassFbo(const std::string& name) {
    auto it = mapRenderPasses.find(name);
    if (it != mapRenderPasses.end()) {
        return it->second->fbo();
    } else {
        Log::error("Failed to get FBO: Render pass '" + name + "' not found.");
        return 0;
    }
}


std::vector<unsigned int> RenderPipeline::getPassFboTexs(const std::string &name){
    auto it = mapRenderPasses.find(name);
    if (it != mapRenderPasses.end()) {
        return it->second->fboTexs();
    } else {
        Log::error("Failed to get FBO: Render pass '" + name + "' not found.");
        return {};
    }
}
