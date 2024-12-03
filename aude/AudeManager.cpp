#include "AudeManager.h"
#include <SDL_audio.h>

namespace Aude {

class Manager::Impl {
public:
  DriverList drivers;
  DriverInfo defaultDriver;

  inline static constexpr bool PLAYBACK{false};
  inline static constexpr bool CAPTURE{true};

  static std::vector<std::string> listDriversNames();
  static DeviceList listDevices(bool isCapture);
  static DeviceInfo defaultDevice(bool isCapture);
};

Manager &Manager::Get() {
  static Manager instance;
  return instance;
}

Manager::Manager() : impl_{std::make_unique<Impl>()} { discoverDrivers(); }

Manager::~Manager() {}

void Manager::discoverDrivers() {
  impl_->drivers.clear();
  // collect
  auto drivers = impl_->listDriversNames();
  for (const auto &drv : drivers) {
    initDriver(drv);
    DriverInfo drvInfo;
    drvInfo.name = drv;
    // playback
    drvInfo.playbackDevices = impl_->listDevices(Impl::PLAYBACK);
    drvInfo.defaultPlaybackDevice = impl_->defaultDevice(Impl::PLAYBACK);
    // capture
    drvInfo.captureDevices = impl_->listDevices(Impl::CAPTURE);
    drvInfo.defaultCaptureDevice = impl_->defaultDevice(Impl::CAPTURE);
    impl_->drivers.append(drvInfo);
    quitDriver();
  }
  // set default
  if (!impl_->drivers.empty()) {
    impl_->defaultDriver = impl_->drivers.first();
  } else {
    impl_->defaultDriver = {};
  }
  impl_->drivers.sort();
}

const DriverList &Manager::drivers() const { return impl_->drivers; }

const DriverInfo &Manager::defaultDriver() const {
  return impl_->defaultDriver;
}

void Manager::initDriver(const std::string &driver) {
  SDL_AudioInit(driver.c_str());
}

void Manager::quitDriver() { SDL_AudioQuit(); }

std::vector<std::string> Manager::Impl::listDriversNames() {
  std::vector<std::string> list;
  const auto n = SDL_GetNumAudioDrivers();
  for (int i{}; i < n; ++i) {
    list.push_back(SDL_GetAudioDriver(i));
  }
  return list;
}

DeviceList Manager::Impl::listDevices(bool isCapture) {
  DeviceList list;
  const auto n = SDL_GetNumAudioDevices(isCapture);
  for (int i{}; i < n; ++i) {
    DeviceInfo info{.name = SDL_GetAudioDeviceName(i, isCapture),
                    .isCapture = isCapture};
    list.append(info);
  }
  list.sort();
  return list;
}

DeviceInfo Manager::Impl::defaultDevice(bool isCapture) {
  DeviceInfo info;
  info.isCapture = isCapture;
  SDL_AudioSpec spec{};
  char *name{};
  SDL_GetDefaultAudioInfo(&name, &spec, isCapture);
  if (name != nullptr) {
    info.name = std::string(name);
    free(name);
  }
  if (info.name.empty()) {
    const auto list = listDevices(isCapture);
    if (!list.empty()) {
      info.name = list.first().name;
    }
  }
  return info;
}

} // namespace Aude
