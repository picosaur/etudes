#pragma once
#include <memory>

namespace EtGui {

class WaveWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  WaveWidget();
  ~WaveWidget();

  void show();
};

} // namespace EtGui
