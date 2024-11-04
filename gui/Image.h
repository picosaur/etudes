#pragma once
#include <imgui.h>
#include <memory>

namespace gui
{
    class Image
    {
        class Impl;
        std::unique_ptr<Impl> impl_;

    public:
        Image();
        Image(const char *data, std::size_t size);
        ~Image();

        int width() const;
        int height() const;

        ImTextureID textureId() const;
        ImVec2 textureSize() const;

    private:
        void loadTexture();
    };
}