#include "EtFireManager.h"
#include <arrayfire.h>
#include <vector>

namespace EtFire {

class Manager::Impl {
public:
  std::vector<BackendInfo> backends;

  std::vector<BackendInfo> listBackends() const;
  std::vector<DeviceInfo> listDevices(af::Backend bknd) const;
};

Manager::Manager() : impl_{std::make_unique<Impl>()} { // backends

  discoverBackends();
}

Manager::~Manager() {}

Manager &Manager::Get() {
  static Manager instance;
  return instance;
}

void Manager::discoverBackends() {
  impl_->backends = impl_->listBackends();
  for (auto &item : impl_->backends) {
    item.devices = impl_->listDevices((af::Backend)item.bknd);
  }
}

const std::vector<BackendInfo> &Manager::backends() const {
  return impl_->backends;
}

std::vector<BackendInfo> Manager::Impl::listBackends() const {
  int v = af::getAvailableBackends();
  std::vector<BackendInfo> list;
  if (v & AF_BACKEND_CPU) {
    list.push_back({.name = "CPU", .bknd = AF_BACKEND_CPU});
  }
  if (v & AF_BACKEND_CUDA) {
    list.push_back({.name = "CUDA", .bknd = AF_BACKEND_CUDA});
  }
  if (v & AF_BACKEND_OPENCL) {
    list.push_back({.name = "OpenCL", .bknd = AF_BACKEND_OPENCL});
  }
  return list;
}

std::vector<DeviceInfo> Manager::Impl::listDevices(af::Backend bknd) const {
  af::setBackend(bknd);

  char name[64];
  char platform[64];
  char toolkit[64];
  char compute[64];

  std::vector<DeviceInfo> list;
  int n{};
  try {
    n = af::getDeviceCount();
  } catch (const std::exception &err) {
  }

  for (int i{}; i < n; ++i) {
    af::setDevice(i);
    af::deviceInfo(name, platform, toolkit, compute);
    list.push_back({.name = name,
                    .platform = platform,
                    .toolkit = toolkit,
                    .compute = compute});
  }
  return list;
}

} // namespace EtFire
