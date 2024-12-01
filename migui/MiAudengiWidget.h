#pragma once
#include <memory>

namespace Mi {

class AudengiWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  AudengiWidget();
  ~AudengiWidget();

  void show();
};

} // namespace Mi
