/**
 * @file Transform.h
 * @brief 3-D spatial transform (position, rotation, scale).
 */
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/**
 * @brief Encapsulates the position, orientation, and scale of a scene object.
 *
 * Rotation is stored as a unit quaternion to avoid gimbal lock.
 */
struct Transform {
    glm::vec3 position {0.f};
    glm::quat rotation {1.f, 0.f, 0.f, 0.f}; ///< Identity quaternion.
    glm::vec3 scale    {1.f};

    /**
     * @brief Builds a column-major model matrix: T * R * S.
     * @return The combined 4×4 model matrix.
     */
    glm::mat4 matrix() const;
};
