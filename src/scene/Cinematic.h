/**
 * @file Cinematic.h
 * @brief Class which contains uniforms to pass to shaders depending on the time.
 */
#pragma once
#include "renderer/Shader.h"

struct Cinematic {
   void uploadUniforms(const Shader& shader, float time);
};
