#pragma once
#include <memory>

namespace Mi {
namespace QdspGui {

// DemoWidget
// ----------------------------------------------------------------------------
class DemoWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DemoWidget();
  ~DemoWidget();

  void show();
};
} // namespace QdspGui
} // namespace Mi
