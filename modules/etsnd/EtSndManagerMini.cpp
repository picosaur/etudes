#if defined(ETSND_USE_MINIAUDIO)
#include "EtSndManager.h"
#include <array>
#include <miniaudio.h>
#include <unordered_map>

namespace EtSnd {

class Manager::Impl {
public:
  DriverList drivers;
  DriverInfo defaultDriver;

  std::unordered_map<std::string, ma_backend> backendsMap;

  void initBackendsMap();
};

Manager &Manager::Get() {
  static Manager instance;
  return instance;
}

Manager::Manager() : impl_{std::make_unique<Impl>()} { discoverDrivers(); }

Manager::~Manager() {}

void Manager::discoverDrivers() {
  impl_->drivers.clear();
  impl_->initBackendsMap();
  impl_->drivers.sort();
}

const DriverList &Manager::drivers() const { return impl_->drivers; }

const DriverInfo &Manager::defaultDriver() const {
  return impl_->defaultDriver;
}

void Manager::initDriver(const std::string &driver) {
  // ma_context_init(impl_->backendsMap.at(driver));
}

void Manager::quitDriver() {}

void Manager::Impl::initBackendsMap() {
  backendsMap.clear();
  std::array<ma_backend, MA_BACKEND_COUNT> list;
  std::size_t count{};
  auto result = ma_get_enabled_backends(list.data(), MA_BACKEND_COUNT, &count);
  if (result == MA_SUCCESS) {
    for (const auto &item : list) {
      if (item == ma_backend_null) {
        break;
      }
      if (item != ma_backend_custom) {
        backendsMap.insert({ma_get_backend_name(item), item});
      }
    }
  }
}

} // namespace EtSnd

#endif
