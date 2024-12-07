#include "MiImage.h"
#include <MiGL.h>
#include <algorithm>
#include <execution>
#include <stb_image.h>

namespace Mi {

// Impl
// ----------------------------------------------------------------------------
class Image::Impl {
public:
  unsigned char *rgba{};
  GLint width{};
  GLint height{};
  GLuint textureId{};
  bool stbi{};

  ImU32 *data() const;
  int size() const;

  auto begin();
  auto end();

  void loadTexture();
  void updateTexture();
  void freeTexture();
};

auto Image::Impl::begin() { return data(); }

auto Image::Impl::end() { return data() + size(); }

ImU32 *Image::Impl::data() const { return reinterpret_cast<ImU32 *>(rgba); }

int Image::Impl::size() const { return width * height; }

void Image::Impl::loadTexture() {
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#if defined(HELLOIMGUI_USE_GLES2) || defined(HELLOIMGUI_USE_GLES3)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, rgba);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::Impl::updateTexture() {
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA,
                  GL_UNSIGNED_BYTE, rgba);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::Impl::freeTexture() {
  if (rgba != nullptr) {
    glDeleteTextures(1, &textureId);
    if (stbi) {
      stbi_image_free(rgba);
    } else {
      delete[] rgba;
    }
    rgba = {};
  }
}

// Image
// ----------------------------------------------------------------------------
Image::Image() : impl_{std::make_unique<Impl>()} {}

Image::Image(int width, int height) : impl_{std::make_unique<Impl>()} {
  impl_->width = width;
  impl_->height = height;
  impl_->rgba = new unsigned char[width * height * 4];
  impl_->loadTexture();
}

Image::Image(const std::byte *data, std::size_t size)
    : impl_{std::make_unique<Impl>()} {
  impl_->rgba = stbi_load_from_memory((unsigned char *)data, size,
                                      &impl_->width, &impl_->height, NULL, 4);
  impl_->stbi = true;
  impl_->loadTexture();
}

Image::~Image() { impl_->freeTexture(); }

void Image::render() { impl_->updateTexture(); }

ImTextureID Image::textureId() const {
  return (ImTextureID)(intptr_t)impl_->textureId;
}

int Image::width() const { return impl_->width; }

int Image::height() const { return impl_->height; }

ImVec2 Image::imageSize() const {
  return {(float)impl_->width, (float)impl_->height};
}

// Modifiers
// ----------------------------------------------------------------------------
void Image::fill(ImU32 color) {
  std::for_each(std::execution::par, impl_->begin(), impl_->end(),
                [&](auto &val) { val = color; });
}

void Image::fillRow(int row, ImU32 color, int col0, int col1) {
  if (row >= height() || col0 >= width() || col1 >= width()) {
    throw(std::out_of_range("Mi::Image::fillRow out of range"));
  }
  col0 = col0 < 0 ? 0 : col0;
  col1 = col1 < 0 ? impl_->width : col1;
  ImU32 *data = impl_->data();
  const auto size = impl_->size();
  const int i0{row * int(impl_->width) + col0};
  const int i1{i0 + col1};
  for (int i{i0}; i < i1; ++i) {
    data[i] = color;
  }
}

void Image::fillCol(int col, ImU32 color, int row0, int row1) {
  if (col >= width() || row0 >= height() || row1 >= height()) {
    throw(std::out_of_range("Mi::Image::fillRow out of range"));
  }
  row0 = row0 < 0 ? 0 : row0;
  row1 = row1 < 0 ? impl_->height : row1;
  ImU32 *data = impl_->data();
  const auto size = impl_->size();
  const int j0{row0 * impl_->width};
  for (int i{row0}, j{j0}; i < row1; ++i, j += impl_->width) {
    data[j + col] = color;
  }
}

void Image::fillRect(ImU32 color) {
  fillRow(0, color);
  fillRow(height() - 1, color);
  fillCol(0, color);
  fillCol(width() - 1, color);
}

} // namespace Mi
