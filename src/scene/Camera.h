/**
 * @file Camera.h
 * @brief Perspective camera producing view and projection matrices.
 */
#pragma once
#include <glm/glm.hpp>

/**
 * @brief A perspective camera defined by position, target, and field of view.
 */
struct Camera {
    glm::vec3 position  {0.f, 1.f, 5.f};
    glm::vec3 target    {0.f, 0.f, 0.f};
    glm::vec3 up        {0.f, 1.f, 0.f};
    float     fov       = 45.f;  ///< Vertical field of view in degrees.
    float     nearPlane = 0.1f;
    float     farPlane  = 100.f;

    /**
     * @brief Computes the view matrix (world → camera space).
     * @return Column-major view matrix.
     */
    glm::mat4 view() const;

    /**
     * @brief Computes the projection matrix (camera → clip space).
     * @param aspect Viewport width / height.
     * @return Column-major perspective projection matrix.
     */
    glm::mat4 projection(float aspect) const;
};
