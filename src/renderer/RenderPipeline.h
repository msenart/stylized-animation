//
// Created by mathi on 16/05/2026.
//

#ifndef STYLIZE_ANIMATION_RENDERPIPELINE_H
#define STYLIZE_ANIMATION_RENDERPIPELINE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
/**
 * @brief Virtual class that gathers all the types of render passes.
 */
class RenderPass {

    virtual void setup(unsigned int window_w, unsigned int window_h) = 0;
    virtual void execute() = 0;
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
    void setup(unsigned int window_w, unsigned int window_h) override;
    void execute() override;
    ~MeshIDRenderPass() override = default;

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