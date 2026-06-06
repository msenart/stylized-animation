#pragma once
#include "scene/Scene.h"
#include "core/Window.h"

/**
 * @brief It is a bag containing the necessary information about how the scene has to be rendered.
 */
struct RenderContext {
    Camera* camera;
    Scene* scene;
    Window* window;
    PassTag passTag;
    float deltaTime;
};