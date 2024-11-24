#pragma once
#include <memory>

namespace Mi {
class HttpFetchWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  HttpFetchWidget();
  ~HttpFetchWidget();

  void show();
};
} // namespace Mi