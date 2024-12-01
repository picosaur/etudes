#include "Audengi.h"
#include <SDL_audio.h>

namespace Audengi {

class Engine::Impl {
public:
  static auto getDrivers() {
    std::vector<std::string> list;
    const auto n = SDL_GetNumAudioDrivers();
    for (int i{}; i < n; ++i) {
      list.push_back(SDL_GetAudioDriver(i));
    }
    return list;
  }

  static auto getDevices(bool iscapture) {
    std::vector<DeviceInfo> list;
    const auto n = SDL_GetNumAudioDevices(iscapture);
    for (int i{}; i < n; ++i) {
      DeviceInfo info;
      info.name = SDL_GetAudioDeviceName(i, iscapture);
      list.push_back(std::move(info));
    }
    return list;
  }
};

Engine::Engine() {}

Engine::~Engine() {}

void Engine::init(const std::string &driver) {}

void Engine::quit() {}

std::vector<std::string> Engine::audioDrivers() const {
  return impl_->getDrivers();
}

std::vector<DeviceInfo> Engine::playbackDevices() const {
  return impl_->getDevices(false);
}

std::vector<DeviceInfo> Engine::recordingDevices() const {
  return impl_->getDevices(true);
}

} // namespace Audengi
