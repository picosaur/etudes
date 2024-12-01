#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Audengi {

// DeviceInfo
// ----------------------------------------------------------------------------
class DeviceInfo {
public:
  std::string name{};
  int channum{};
  int silence{};
  int samples{};
  int padding{};
  int bufsize{};
};
class DeviceList : public std::vector<DeviceInfo> {};

// DriverInfo
// ----------------------------------------------------------------------------
class DriverInfo {
public:
  std::string name{};
  DeviceList playbackDevices;
  DeviceList recordingDevices;
};
class DriverList : public std::vector<DriverInfo> {};

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

  static Manager &Get() {
    static Manager instance;
    return instance;
  }

  void initDriver(const std::string &driver);
  void quitDriver();

  std::vector<std::string> listDrivers() const;
  DeviceList listPlaybackDevices() const;
  DeviceList listRecordingDevices() const;

  const DriverList &discoverAndTest();
  const DriverList &allDevices() const;
};

} // namespace Audengi
