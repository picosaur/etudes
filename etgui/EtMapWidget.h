#pragma once
#include <memory>

namespace EtGui {
class MapWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  MapWidget();
  ~MapWidget();

  void show();
};
} // namespace EtGui
