#pragma once
#include <imgui.h>
#include <implot.h>
#include <memory>
#include <vector>

namespace EtGui {
class Image {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  Image();
  Image(int width, int height);
  Image(const std::byte *data, std::size_t size);
  ~Image();

  void render();
  ImTextureID textureId() const;

  // Geometry
  int width() const;
  int height() const;
  ImVec2 imageSize() const;

  // Analyze
  std::vector<int> findColor(ImU32 color) const;

  // Modifiers
  void fill(ImU32 color);
  void fillRow(int row, ImU32 color, int col0 = -1, int col1 = -1);
  void fillCol(int col, ImU32 color, int row0 = -1, int row1 = -1);
  void fillRect(ImU32 color);
  void fillColor(const std::vector<int> &indexes, ImU32 color);
};
} // namespace EtGui
