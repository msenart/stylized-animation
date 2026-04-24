/**
 * @file Light.h
 * @brief Light source data structure.
 */
#pragma once
#include <glm/glm.hpp>

/**
 * @brief Classification of a light source.
 */
enum class LightType { Point, Directional };

/**
 * @brief A light source placed in the scene.
 */
struct Light {
    LightType type      = LightType::Point;
    glm::vec3 position  {0.f, 5.f, 3.f};  ///< World-space position (Point lights).
    glm::vec3 direction {0.f, -1.f, 0.f}; ///< Normalised direction (Directional lights).
    glm::vec3 color     {1.f};
    float     intensity = 1.f;
};
