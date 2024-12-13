#pragma once
#include <memory>

namespace EtGui {
namespace DspGui {

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
} // namespace DspGui
} // namespace EtGui
