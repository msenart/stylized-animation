//
// Created by mathi on 16/05/2026.
//

#include "RenderPipeline.h"

#include "core/Log.h"
#include "glad/glad.h"

// render passes

// MeshIDRenderPass implementation

void MeshIDRenderPass::setup(unsigned int window_h, unsigned int window_w) {
    // creating frame buffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // creating texture for the frame buffer
    glGenTextures(1, &m_fboTex);
    glBindTexture(GL_TEXTURE_2D, m_fboTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, window_w, window_h, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTex, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        Log::error("Framebuffer "+ tag +"  could not be created : " + std::to_string(fboStatus));
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

unsigned int FinalRenderPass::fbo() {return 0;}

unsigned int FinalRenderPass::fboTex() {return 0;}

// render pipeline

void RenderPipeline::addPass(const std::shared_ptr<RenderPass> &pass) {
    std::string& tag = pass->tag;

    if (mapRenderPasses.count(tag) > 0) {
        Log::warn("The pass '" + tag + "' has the same name as another pass already in the render pipeline! Overwriting...");
    }

    mapRenderPasses[tag] = pass;
}

void RenderPipeline::setup(unsigned int window_w, unsigned int window_h) const {
    for (auto& pair : mapRenderPasses) {
        pair.second->setup(window_w,window_h);
    }
}

void RenderPipeline::execute(const std::string& name) {
    mapRenderPasses[name]->execute();
}

void RenderPipeline::clear(const std::string &name) {
    mapRenderPasses[name]->clear();
}

void RenderPipeline::onResize(unsigned int window_w, unsigned int window_h) const {
    for (auto& pair : mapRenderPasses) {
        pair.second->onResize(window_w, window_h);
    }
}

unsigned int RenderPipeline::getPassTextures(const std::string &name) {
    return mapRenderPasses[name]->fboTex();
}
