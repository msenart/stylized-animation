/**
 * Hello ! Here below is the necessary classes to define the rendering pipeline.
 * To make a new rendering pipeline, you should :
 * - Think about a relevant name. See if you can use the information of a previous buffer.
 * In RenderingPipeline.h/.cpp :
 * - Insert that new name in : enum class PassTag, std::string PassTagToString(PassTag tag).
 * - Create a class that is inherited from RenderPass, that have the same core structure as the previous ones (see MeshIDRenderPass).
 * In Renderer.h/.cpp :
 * - Add your new render pass in the rendering pipeline in Renderer::setup()
 * - Like did for the other render passes, add the render pass in the right place (where you think you need the previous information and where that info will be useful for the next passes).
 * In AnimatedMesh.cpp/StaticMesh.cpp (if you want the render pass to be the default pass for these types of meshes) or in main.cpp.
 * - AnimatedMesh.cpp/StaticMesh.cpp : add a new entry in the Mesh::shadersKeyMap() map function.
 * - In main.cpp : When instantiating a new Object type, you can modify the constructor arguments to add your own passes in the member passTagSpecifications. I am not sure if you must not fill the defines variable, because it is dedicated for other functions in the code.
*/

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
    Generic,
    Shadow, // influences the shadow cast pass (not implemented but it's an example)
    MeshID, // can be selected
    Final,  // can be rendered
};

inline std::string PassTagToString(PassTag tag) {
    switch (tag) {
        case PassTag::Generic: return "Generic Render Pass";
        case PassTag::Shadow:  return "Shadow Render Pass";
        case PassTag::MeshID:  return "Mesh ID Render Pass";
        case PassTag::Final:   return "Final Render Pass";
        default:               return "Unknown Pass";
    }
}

/**
 * @brief Virtual class that gathers all the types of render passes.
 */
class RenderPass {
public:
    /**
     * @brief Called once at the beginning of the execution. sets up all the necessary variables (especially OpenGL) to be operational.
     * @param window_w window width
     * @param window_h window height
     */
    virtual void setup(unsigned int window_w, unsigned int window_h) = 0;

    /**
     * @brief Called at the beginning of the pass to activate the fbo and the right rendering parameters (GL_CULL_FACE, GL_DEPTH_TEST...)
     */
    virtual void execute() = 0;

    /**
     * @brief clear the buffer values.
     */
    virtual void clear() = 0;

    /**
     * @brief Specific callback automatically called when the window is resized by the user.
     * @param window_w window width.
     * @param window_h window height.
     */
    virtual void onResize(unsigned int window_w, unsigned int window_h) {
        setup(window_w, window_h);
    }

    /**
     *
     * @return the fbo of the rendering pass.
     */
    virtual unsigned int fbo() = 0;

    /**
     *
     * @return the precise name of the rendering pass. See MeshIDRenderPass::name() for example.
     */
    virtual std::string name() = 0;

    virtual ~RenderPass() = default;

    explicit RenderPass(std::string suffix) : m_suffix(suffix) {}

    RenderPass() = default;

    PassTag tag = PassTag::Generic;
    std::string m_suffix;
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

    std::string name() {
        return !m_suffix.empty() ? PassTagToString(tag) + " " + m_suffix : PassTagToString(tag);
    }

    unsigned int fboTex() const {
        return m_fboTex;
    }
    MeshIDRenderPass(std::string suffix) : RenderPass(std::move(suffix)) {
        tag = PassTag::MeshID;
    };

    MeshIDRenderPass() : RenderPass() {
        tag = PassTag::MeshID;
    }

    void setup(unsigned int window_w, unsigned int window_h) override;
    void execute() override;
    ~MeshIDRenderPass() override = default;
    void clear() override;

    void onResize(unsigned int window_w, unsigned int window_h) override;
};

/**
 * @brief Render pass that draws what is on the screen.
 */
class FinalRenderPass final : public RenderPass {
public:
    explicit FinalRenderPass(std::string suffix) : RenderPass(suffix) {
        tag = PassTag::Final;
    };

    FinalRenderPass() : RenderPass() {
        tag = PassTag::Final;
    }

    void setup(unsigned window_w, unsigned window_h) override;

    void execute() override ;

    void clear() override;

    void onResize(unsigned int window_w, unsigned int window_h) override;

    unsigned int fbo() override {
        return 0;
    }

    std::string name() {
        return !m_suffix.empty() ? PassTagToString(tag) + " " + m_suffix : PassTagToString(tag);
    }

    unsigned int fboTex() {
        return 0;
    }
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

    unsigned int getPassFbo(const std::string &name);
};