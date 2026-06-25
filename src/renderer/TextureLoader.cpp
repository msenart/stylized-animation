#include "renderer/TextureLoader.h"
#include "core/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static GLuint uploadToGL(unsigned char* data, int w, int h) {
    GLuint texID;
    glCreateTextures(GL_TEXTURE_2D, 1, &texID);
    glTextureStorage2D(texID, 1, GL_RGBA8, w, h);
    glTextureSubImage2D(texID, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateTextureMipmap(texID);
    glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(texID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return texID;
}

GLuint TextureLoader::loadFromFile(const std::string& path) {
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data) {
        Log::error("TextureLoader: failed to load '" + path + "': " + stbi_failure_reason());
        return 0;
    }
    GLuint id = uploadToGL(data, w, h);
    stbi_image_free(data);
    return id;
}

GLuint TextureLoader::loadFromMemory(const unsigned char* buf, int byteSize) {
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load_from_memory(buf, byteSize, &w, &h, &ch, 4);
    if (!data) {
        Log::error(std::string("TextureLoader: failed to decode from memory: ") + stbi_failure_reason());
        return 0;
    }
    GLuint id = uploadToGL(data, w, h);
    stbi_image_free(data);
    return id;
}
