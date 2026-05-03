/**
 * @file AssetManager.h
 * @brief Registry that owns GPU mesh resources and issues opaque handles.
 */
#pragma once
#include <memory>
#include <unordered_map>
#include "core/Types.h"
#include "renderer/Mesh.h"

/**
 * @brief Owns all Mesh objects and provides access via MeshHandle.
 *
 * A MeshHandle is an opaque integer; it is stable for the lifetime of
 * the AssetManager. Meshes cannot be removed once added.
 */
class AssetManager {
public:
    /**
     * @brief Takes ownership of a mesh and registers it.
     * @param mesh Heap-allocated mesh to store (ownership transferred).
     * @return A stable handle for later retrieval.
     */
    MeshHandle add(std::unique_ptr<Mesh> mesh);

    /**
     * @brief Returns a const reference to the mesh for a given handle.
     * @param handle A handle previously returned by add().
     * @return reference to the mesh.
     * @throws std::runtime_error if the handle is unknown.
     */
    Mesh& get(MeshHandle handle) const;

    /// @brief Returns the total number of meshes currently stored.
    std::size_t meshCount() const;

private:
    std::unordered_map<MeshHandle, std::unique_ptr<Mesh>> m_meshes;
    MeshHandle m_next = 1;
};