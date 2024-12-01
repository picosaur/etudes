#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Audengi {

class DeviceInfo {
public:
  std::string name{};
  int channum{};
  int silence{};
  int samples{};
  int padding{};
  int bufsize{};
};

class Engine {
  Engine();
  ~Engine();

  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  Engine(const Engine &) = delete;
  Engine &operator=(Engine &) = delete;

  static Engine &Get() {
    static Engine instance;
    return instance;
  }

  void init(const std::string &driver);
  void quit();

  std::vector<std::string> audioDrivers() const;
  std::vector<DeviceInfo> playbackDevices() const;
  std::vector<DeviceInfo> recordingDevices() const;
};

} // namespace Audengi
