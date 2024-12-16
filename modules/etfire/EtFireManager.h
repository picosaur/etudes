#pragma once
#include <memory>
#include <string>
#include <vector>

namespace EtFire {

// DeviceInfo
// ----------------------------------------------------------------------------
class DeviceInfo {
public:
  std::string name{};
  std::string platform{};
  std::string toolkit{};
  std::string compute{};
};

// BackendInfo
// ----------------------------------------------------------------------------
class BackendInfo {
public:
  std::string name{};
  int bknd{};
  std::vector<DeviceInfo> devices;
};

// Manager
// ----------------------------------------------------------------------------
class Manager {
  Manager();
  ~Manager();

  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  Manager(const Manager &) = delete;
  Manager &operator=(Manager &) = delete;

  static Manager &Get();

  void discoverBackends();
  const std::vector<BackendInfo> &backends() const;
};

} // namespace EtFire
