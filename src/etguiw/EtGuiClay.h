#pragma once
#include <memory>

namespace EtGuiw {
namespace ClayGui {

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
} // namespace ClayGui
} // namespace EtGuiw
