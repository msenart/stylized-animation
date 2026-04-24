/**
 * @file Geometry.h
 * @brief Procedural mesh generators producing CPU-side MeshData.
 */
#pragma once
#include <vector>
#include <cstdint>
#include "renderer/Mesh.h"

/**
 * @brief Factory functions that return raw vertex/index data ready for Mesh upload.
 */
namespace Geometry {

/**
 * @brief CPU-side mesh data: vertices and triangle indices.
 */
struct MeshData {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
};

/**
 * @brief Generates a unit cube centred at the origin.
 *
 * 24 vertices (4 per face for correct per-face normals) and 36 indices.
 * @return MeshData ready to be passed to the Mesh constructor.
 */
MeshData makeCube();

} // namespace Geometry
