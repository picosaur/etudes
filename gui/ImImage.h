#pragma once
#include <imgui.h>
#include <memory>

class ImImage
{
    class Impl;
    std::unique_ptr<Impl> impl_;

public:
    ImImage();
    ImImage(const std::byte* data, std::size_t size);
    ~ImImage();

    ImTextureID textureId() const;
    ImVec2 size() const;

private:
    void loadTexture();
    void freeTexture();
};
