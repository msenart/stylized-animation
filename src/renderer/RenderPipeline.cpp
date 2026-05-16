//
// Created by mathi on 16/05/2026.
//

#include "RenderPipeline.h"

#include "core/Log.h"
#include "glad/glad.h"

void MeshIDRenderPass::setup(unsigned int window_h, unsigned int window_w) {
    // creating frame buffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // creating texture for the frame buffer
    glGenTextures(1, &fboTex);
    glBindTexture(GL_TEXTURE_2D, fboTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, window_w, window_h, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        Log::error("Framebuffer "+ name +"  could not be created : " + std::to_string(fboStatus));
    }
}

void MeshIDRenderPass::execute() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
}

void MeshIDRenderPass::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    GLuint clearValue = 0;
    glClearBufferuiv(GL_COLOR, 0, &clearValue);
}
