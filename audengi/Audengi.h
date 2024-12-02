#pragma once
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace Audengi {

// KeyList
// ----------------------------------------------------------------------------
template <typename T> class KeyList {
  std::vector<T> list_;

public:
  auto begin() { return list_.begin(); }
  auto end() { return list_.end(); }
  const auto &first() const { return list_.front(); }
  const auto &last() const { return list_.back(); }
  auto empty() const { return list_.empty(); }
  void append(const T &item) { list_.push_back(item); }

  auto find(const std::string &name) const {
    return std::find_if(list_.begin(), list_.end(),
                        [&](const auto &item) { return name == item.name; });
  }

  const auto &at(const std::string &name) const {
    const auto it{find(name)};
    if (it == list_.end()) {
      throw(std::out_of_range("Audengi::KeyList::at"));
    }
    return (*it);
  }

  const auto contains(const std::string &name) const {
    return find(name) != list_.end();
  }

  void clear() { list_.clear(); }

  void sort() {
    std::sort(list_.begin(), list_.end(), [](const auto &lhs, const auto &rhs) {
      return lhs.name < rhs.name;
    });
  }
};

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

class DeviceList : public KeyList<DeviceInfo> {};

// DriverInfo
// ----------------------------------------------------------------------------
class DriverInfo {
public:
  std::string name{};
  DeviceList playbackDevices;
  DeviceList recordingDevices;
};

class DriverList : public KeyList<DriverInfo> {};

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

  void discoverDrivers();
  const DriverList &drivers() const;

  void initDriver(const std::string &driver);
  void quitDriver();
};

} // namespace Audengi
