/**
 * @file Types.h
 * @brief Project-wide type aliases.
 */
#pragma once
#include <cstdint>
#include <string>

/// @brief Opaque handle to a mesh stored in the AssetManager. (Returned by AssetManager.add)
using MeshHandle = uint32_t;

/// @brief the path to the shader folder.
static const std::string SHADER_FOLDER_PATH = "./shaders/";

