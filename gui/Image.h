#pragma once
#include <imgui.h>
#include <memory>

namespace Gui
{
    class Image
    {
        class Impl;
        std::unique_ptr<Impl> impl_;

    public:
        Image();
        Image(const std::byte* data, std::size_t size);
        ~Image();

        ImTextureID textureId() const;
        ImVec2 size() const;

    private:
        void loadTexture();
        void freeTexture();
    };
}