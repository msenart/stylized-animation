//
// Created by mathi on 16/05/2026.
//

#include "RenderPipeline.h"

#include "core/Log.h"
#include "glad/glad.h"

// render passes

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

// final render pass

void FinalRenderPass::setup(unsigned window_w, unsigned window_h) {}

void FinalRenderPass::execute() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
}

void FinalRenderPass::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// render pipeline

void RenderPipeline::addPass(const std::shared_ptr<RenderPass> &pass) {
    std::string name = pass->name();

    if (mapRenderPasses.count(name) > 0) {
        Log::warn("The pass '" + name + "' has the same name as another pass already in the render pipeline! Overwriting...");
    }

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
