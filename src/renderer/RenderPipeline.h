//
// Created by mathi on 16/05/2026.
//

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <unordered_map>

#include "core/Log.h"

/**
 * @brief Identifies which Object influences which pass
 */
enum class PassTag {
    GenericRenderPass,
    ShadowRenderPass, // influences the shadow cast pass (not implemented but it's an example)
    MeshIDRenderPass, // can be selected
    FinalRenderPass, // can be rendered
};

/**
 * @brief Virtual class that gathers all the types of render passes.
 */
class RenderPass {
public:
    virtual void setup(unsigned int window_w, unsigned int window_h) = 0;
    virtual void execute() = 0;
    virtual void clear() = 0;
    virtual void onResize(unsigned int window_w, unsigned int window_h) {
        setup(window_w, window_h);
    }
    virtual unsigned int fbo() = 0;
    virtual unsigned int fboTex() = 0;

    virtual ~RenderPass() = default;
    std::string tag = "GenericRenderTag";
};

/**
 * @brief Render pass that draws a mesh ID texture map to make mesh selection easier.
 */
class MeshIDRenderPass final : public RenderPass {
    unsigned int m_fbo = 0;
    unsigned int m_fboTex = 0;
public:
    unsigned int fbo() override {
        return m_fbo;
    }
    unsigned int fboTex() override {
        return m_fboTex;
    }
    MeshIDRenderPass() : RenderPass() {
        tag = "MeshIDRenderPass";
    };
    void setup(unsigned int window_w, unsigned int window_h) override;
    void execute() override;
    ~MeshIDRenderPass() override = default;
    void clear() override;

};

/**
 * @brief Render pass that draws what is on the screen.
 */
class FinalRenderPass final : public RenderPass {
public:
    FinalRenderPass(): RenderPass() {
        tag = "FinalRenderPass";
    };

    void setup(unsigned window_w, unsigned window_h) override;

    void execute() override ;

    void clear() override;

    unsigned int fbo() override;

    unsigned int fboTex() override;
};

/**
 * @brief Applies different render passes to fill the buffers.
 */
class RenderPipeline {
    std::unordered_map<std::string, std::shared_ptr<RenderPass>> mapRenderPasses;
public:
    void addPass(const std::shared_ptr<RenderPass> &pass);

    void setup(unsigned int window_w, unsigned int window_h) const;

    void execute(const std::string& name);

    void clear(const std::string& name);

    void onResize(unsigned int window_w, unsigned int window_h) const;

    unsigned int getPassTextures(const std::string &name);
};