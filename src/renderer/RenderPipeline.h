//
// Created by mathi on 16/05/2026.
//

#ifndef STYLIZE_ANIMATION_RENDERPIPELINE_H
#define STYLIZE_ANIMATION_RENDERPIPELINE_H

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <unordered_map>

/**
 * @brief Identifies which Object influences which pass
 */
enum class PassTag {
    CastShadow, // influences the shadow cast pass (not implemented but it's an example)
    Selectable, // can be selected
    Renderable, // can be rendered
};

/**
 * @brief Virtual class that gathers all the types of render passes.
 */
class RenderPass {

    virtual void setup(unsigned int window_w, unsigned int window_h) = 0;
    virtual void execute() = 0;
    virtual std::set<std::string> getShaderDefines() = 0;
    virtual void clear() = 0;
    virtual void onResize(unsigned int window_w, unsigned int window_h) {
        setup(window_w, window_h);
    }
public:
    virtual ~RenderPass() = default;
    std::string name;
};

/**
 * @brief Render pass that draws a mesh ID texture map to make mesh selection easier
 */
class MeshIDRenderPass final : public RenderPass {
    unsigned int fbo = 0;
    unsigned int fboTex = 0;
    const static std::set<std::string> SHADER_DEFINES;

    void setup(unsigned int window_w, unsigned int window_h) override;
    void execute() override;
    ~MeshIDRenderPass() override = default;
    std::set<std::string> getShaderDefines() override {
        return SHADER_DEFINES;
    }
    void clear() override;
};

/**
 * @brief Applies different render passes to fill the buffers.
 */
class RenderPipeline {
public:
    std::vector<std::unique_ptr<RenderPass>> renderPasses;
    std::unordered_map<std::string, unsigned int> passTextures;
    void addPass(std::unique_ptr<RenderPass> pass);
    void removePass(std::string name);
    void execute();
    unsigned int getPassTextures(const std::string &name) {
        return passTextures[name];
    };
};


#endif //STYLIZE_ANIMATION_RENDERPIPELINE_H