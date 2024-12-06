#pragma once
#include <imgui.h>
#include <memory>

namespace Mi {
class ImageBuffer {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  ImageBuffer(int width, int height);
  ~ImageBuffer();

  void fill();
  void render();

  ImTextureID textureId() const;
  ImVec2 size() const;
};
} // namespace Mi
