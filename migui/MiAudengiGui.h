#pragma once
#include <memory>
#include <string>

namespace Audengi {
class DriverInfo;
class DeviceInfo;
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

  bool show();
  void setList(const Audengi::DriverList &list);
  bool hasCurr() const;
  const Audengi::DriverInfo &currDriver() const;
};

// DeviceCombo
// ----------------------------------------------------------------------------
class DeviceCombo {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DeviceCombo(const std::string &label = "Device");
  ~DeviceCombo();

  bool show();
  void setList(const Audengi::DeviceList &list);
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
