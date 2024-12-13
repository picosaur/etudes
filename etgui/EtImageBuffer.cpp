#include "EtImageBuffer.h"
#include "EtGuiGl.h"

// https://stackoverflow.com/questions/9863969/updating-a-texture-in-opengl-with-glteximage2d
// https://stackoverflow.com/questions/3887636/how-to-manipulate-texture-content-on-the-fly/10702468#10702468

namespace EtGui {
class ImageBuffer::Impl {
public:
  unsigned char *rgba{};
  GLuint fb{};
  GLuint textureId{};
  GLuint width{};
  GLuint height{};

  void initFramebuf();
  void freeFramebuf();

  void loadTexture();
  void freeTexture();
};

ImageBuffer::ImageBuffer(int width, int height)
    : impl_{std::make_unique<Impl>()} {
  impl_->width = width;
  impl_->height = height;
  impl_->rgba = new unsigned char[width * height * 4];
  impl_->loadTexture();
  impl_->initFramebuf();
}

ImageBuffer::~ImageBuffer() {}

void ImageBuffer::fill() {
  int len = (impl_->width * impl_->height);
  ImU32 *data = reinterpret_cast<ImU32*>(impl_->rgba);
  for (int i{}; i < len; ++i) {
    data[i] = IM_COL32_WHITE;
  }
}

void ImageBuffer::render() {
  // render to FBO
  glBindFramebuffer(GL_FRAMEBUFFER, impl_->fb);
  glViewport(0, 0, impl_->width, impl_->height);
  fill();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImTextureID ImageBuffer::textureId() const {
  return (ImTextureID)(intptr_t)impl_->textureId;
}

void ImageBuffer::Impl::initFramebuf() {
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureId, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ImageBuffer::Impl::freeFramebuf() {
  if (fb != 0) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fb);
    fb = {};
  }
}

void ImageBuffer::Impl::loadTexture() {
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if defined(HELLOIMGUI_USE_GLES2) || defined(HELLOIMGUI_USE_GLES3)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, rgba);
}

void ImageBuffer::Impl::freeTexture() {
  if (rgba != nullptr) {
    glDeleteTextures(1, &textureId);
    delete[] rgba;
    rgba = {};
  }
}

} // namespace EtGui
