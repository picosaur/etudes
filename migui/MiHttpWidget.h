#pragma once
#include <memory>

namespace Mi {
class HttpWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  HttpWidget();
  ~HttpWidget();

  void show();
};
} // namespace Mi