#pragma once

#include "renderer/Renderer.h"
#include "ui/SelectionManager.h"
#include "scene/Scene.h"
struct MegaWindowContext {
    Renderer* renderer = nullptr;
    SelectionManager* selectionManager = nullptr;
    Window* window = nullptr;
    Scene* scene = nullptr;
};