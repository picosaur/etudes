#include "Audengi.h"
#include <SDL_audio.h>

namespace Audengi {

class Manager::Impl {
public:
  DriverList drivers;

  static auto listDrivers() {
    std::vector<std::string> list;
    const auto n = SDL_GetNumAudioDrivers();
    for (int i{}; i < n; ++i) {
      list.push_back(SDL_GetAudioDriver(i));
    }
    return list;
  }

  static auto listDevices(bool iscapture) {
    DeviceList list;
    const auto n = SDL_GetNumAudioDevices(iscapture);
    for (int i{}; i < n; ++i) {
      DeviceInfo info;
      info.name = SDL_GetAudioDeviceName(i, iscapture);
      list.append(std::move(info));
    }
    list.sort();
    return list;
  }
};

Manager &Manager::Get() {
  static Manager instance;
  return instance;
}

Manager::Manager() : impl_{std::make_unique<Impl>()} {}

Manager::~Manager() {}

void Manager::discoverDrivers() {
  impl_->drivers.clear();
  auto drivers = impl_->listDrivers();
  for (const auto &drv : drivers) {
    initDriver(drv);
    DriverInfo drvInfo;
    drvInfo.name = drv;
    drvInfo.playbackDevices = impl_->listDevices(false);
    drvInfo.recordingDevices = impl_->listDevices(true);
    impl_->drivers.append(drvInfo);
    quitDriver();
  }
  impl_->drivers.sort();
}

const DriverList &Manager::drivers() const { return impl_->drivers; }

void Manager::initDriver(const std::string &driver) {
  SDL_AudioInit(driver.c_str());
}

void Manager::quitDriver() {}

} // namespace Audengi
