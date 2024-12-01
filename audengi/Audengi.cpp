#include "Audengi.h"
#include <SDL_audio.h>

namespace Audengi {

class Manager::Impl {
public:
  DriverList allDevices;

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
      list.push_back(std::move(info));
    }
    return list;
  }
};

Manager::Manager() : impl_{std::make_unique<Impl>()} {}

Manager::~Manager() {}

void Manager::initDriver(const std::string &driver) {
  SDL_AudioInit(driver.c_str());
}

void Manager::quitDriver() {}

std::vector<std::string> Manager::listDrivers() const {
  return impl_->listDrivers();
}

DeviceList Manager::listPlaybackDevices() const {
  return impl_->listDevices(false);
}

DeviceList Manager::listRecordingDevices() const {
  return impl_->listDevices(true);
}

const DriverList &Manager::discoverAndTest() {
  auto drivers = listDrivers();
  for (const auto &drv : drivers) {
    initDriver(drv);
    DriverInfo drvInfo;
    drvInfo.name = drv;
    drvInfo.playbackDevices = listPlaybackDevices();
    drvInfo.recordingDevices = listRecordingDevices();
    impl_->allDevices.push_back(drvInfo);
    quitDriver();
  }
  return impl_->allDevices;
}

const DriverList &Manager::allDevices() const { return impl_->allDevices; }

} // namespace Audengi
