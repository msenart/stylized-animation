/**
 * @file Light.h
 * @brief Light source data structure.
 */
#pragma once
#include <glm/glm.hpp>

/**
 * @brief A light source placed in the scene.
 */
struct alignas(16) Light {
    glm::vec3 position  {0.f, 5.f, 3.f};
    float pad1;
    glm::vec3 color     {1.f};
    float pad2;
    float     intensity = 1.f;
};
