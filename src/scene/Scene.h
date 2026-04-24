/**
 * @file Scene.h
 * @brief Scene container: camera, lights, and all renderable objects.
 */
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "scene/Camera.h"
#include "scene/Light.h"
#include "scene/Transform.h"
#include "core/Types.h"

/**
 * @brief Surface appearance properties for an object.
 */
struct Material {
    glm::vec3 color     {1.f};
    float     roughness = 0.5f;
};

/**
 * @brief A mesh instance in the scene with its own transform and material.
 */
struct Object {
    MeshHandle meshHandle = 0; ///< Reference to geometry in the AssetManager.
    Transform  transform;
    Material   material;
};

/**
 * @brief Top-level scene graph: owns the camera, all lights, and all objects.
 *
 * The Renderer reads from the Scene; it never writes to it.
 */
struct Scene {
    Camera              camera;
    std::vector<Light>  lights;
    std::vector<Object> objects;
};
