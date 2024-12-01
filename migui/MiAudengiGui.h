#pragma once
#include <memory>

namespace Audengi {
class DeviceList;
class DriverList;
} // namespace Audengi

namespace Mi {
namespace AudengiGui {

// DriverCombo
// ----------------------------------------------------------------------------
class DriverCombo {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DriverCombo();
  ~DriverCombo();

  void show();
  void setList(const Audengi::DriverList &list);
};

// DeviceCombo
// ----------------------------------------------------------------------------
class DeviceCombo {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DeviceCombo();
  ~DeviceCombo();

  void show();
  void update();
};

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

} // namespace AudengiGui

} // namespace Mi
