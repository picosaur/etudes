#if defined(ETSND_USE_DUMMY)
#include "EtSndManager.h"
namespace EtSnd {

class Manager::Impl {
public:
  DriverList drivers;
  DriverInfo defaultDriver;
};

Manager &Manager::Get() {
  static Manager instance;
  return instance;
}

Manager::Manager() : impl_{std::make_unique<Impl>()} { discoverDrivers(); }

Manager::~Manager() {}

void Manager::discoverDrivers() {}

const DriverList &Manager::drivers() const { return impl_->drivers; }

const DriverInfo &Manager::defaultDriver() const {
  return impl_->defaultDriver;
}

void Manager::initDriver(const std::string &driver) {}

void Manager::quitDriver() {}

} // namespace EtSnd

#endif
