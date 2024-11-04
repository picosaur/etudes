#include "Image.h"
#include <stb_image.h>
#include <hello_imgui/hello_imgui_include_opengl.h>

namespace gui
{
    class Image::Impl
    {
    public:
        unsigned char *rgba{};
        int width{};
        int height{};
        ImTextureID textureId{};
    };

    Image::Image() {}

    Image::Image(const char *data, std::size_t size) : impl_{std::make_unique<Impl>()}
    {
        impl_->rgba = stbi_load_from_memory((unsigned char *)data, size, &impl_->width, &impl_->height, NULL, 4);
        loadTexture();
    }

    Image::~Image() {}

    int Image::width() const { return impl_->width; }

    int Image::height() const { return impl_->height; }

    ImTextureID Image::textureId() const
    {
        return impl_->textureId;
    }

    ImVec2 Image::textureSize() const
    {
        return {(float)impl_->width, (float)impl_->height};
    }

    void Image::loadTexture()
    {
        GLuint textureId{};
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(HELLOIMGUI_USE_GLES2) || defined(HELLOIMGUI_USE_GLES3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     impl_->width,
                     impl_->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, impl_->rgba);
        impl_->textureId = (ImTextureID)(intptr_t)textureId;
    }
}