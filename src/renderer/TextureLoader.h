#pragma once
#include <glad/glad.h>
#include <string>

namespace TextureLoader {
    /// Loads an image from disk into a GL texture. Returns 0 on failure.
    GLuint loadFromFile(const std::string& path);

    /// Loads a compressed image (PNG/JPG/…) from a memory buffer. Returns 0 on failure.
    GLuint loadFromMemory(const unsigned char* data, int byteSize);
}
