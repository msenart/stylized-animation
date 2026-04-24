#include "core/AssetManager.h"
#include <stdexcept>

MeshHandle AssetManager::add(std::unique_ptr<Mesh> mesh) {
    MeshHandle handle = m_next++;
    m_meshes[handle]  = std::move(mesh);
    return handle;
}

const Mesh& AssetManager::get(MeshHandle handle) const {
    auto it = m_meshes.find(handle);
    if (it == m_meshes.end())
        throw std::runtime_error("AssetManager: unknown MeshHandle");
    return *it->second;
}

std::size_t AssetManager::meshCount() const {
    return m_meshes.size();
}
