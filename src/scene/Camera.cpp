#include "scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::view() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::projection(float aspect) const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}
