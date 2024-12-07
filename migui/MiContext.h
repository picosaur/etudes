#pragma once
#include <imgui_internal.h>

namespace Mi {

template <typename T> class Context {
  Context() {}

public:
  Context(Context const &) = delete;
  void operator=(Context const &) = delete;

  static Context *Get() {
    static Context instance;
    return &instance;
  }

  ImPool<T> pool;
};

} // namespace Mi
