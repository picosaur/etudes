#include "MiAudengiGui.h"
#include "Audengi.h"
#include <imgui.h>

namespace Mi {
namespace AudengiGui {

// DriverCombo
// ----------------------------------------------------------------------------
class DriverCombo::Impl {
public:
  Audengi::DriverList list;
  std::string currName;
};

DriverCombo::DriverCombo() : impl_{std::make_unique<Impl>()} {}

DriverCombo::~DriverCombo() {}

bool DriverCombo::show() {
  bool changed{};
  ImGui::PushID(this);
  if (ImGui::BeginCombo("Driver", impl_->currName.c_str())) {
    for (const auto &item : impl_->list) {
      if (ImGui::Selectable(item.name.c_str(), item.name == impl_->currName)) {
        impl_->currName = item.name;
        changed = true;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
  return changed;
}

void DriverCombo::setList(const Audengi::DriverList &list) {
  impl_->list = list;
  if (impl_->list.empty()) {
    impl_->currName = {};
  } else if (!impl_->list.contains(impl_->currName)) {
    impl_->currName = impl_->list.first().name;
  }
}

bool DriverCombo::hasCurr() const { return !impl_->currName.empty(); }

const Audengi::DriverInfo &DriverCombo::currDriver() const {
  return impl_->list.at(impl_->currName);
}

// DeviceCombo
// ----------------------------------------------------------------------------
class DeviceCombo::Impl {
public:
  std::string label;
  Audengi::DeviceList list;
  std::string currName;
};

DeviceCombo::DeviceCombo(const std::string &label)
    : impl_{std::make_unique<Impl>()} {
  impl_->label = label;
}

DeviceCombo::~DeviceCombo() {}

bool DeviceCombo::show() {
  bool changed{};
  ImGui::PushID(this);
  if (ImGui::BeginCombo(impl_->label.c_str(), impl_->currName.c_str())) {
    for (const auto &item : impl_->list) {
      if (ImGui::Selectable(item.name.c_str(), item.name == impl_->currName)) {
        impl_->currName = item.name;
        changed = true;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
  return changed;
}

void DeviceCombo::setList(const Audengi::DeviceList &list) {
  impl_->list = list;
  if (impl_->list.empty()) {
    impl_->currName = {};
  } else if (!impl_->list.contains(impl_->currName)) {
    impl_->currName = impl_->list.first().name;
  }
}

// DemoWidget
// ----------------------------------------------------------------------------
class DemoWidget::Impl {
public:
  DriverCombo driverCombo;
  DeviceCombo inpDeviceCombo{"Recording"}, outDeviceCombo{"Playback"};

  void updateDriverCombo() {
    auto &man = Audengi::Manager::Get();
    man.discoverAndTest();
    driverCombo.setList(man.drivers());
  }

  void updateDeviceCombo() {
    if (driverCombo.hasCurr()) {
      outDeviceCombo.setList(driverCombo.currDriver().playbackDevices);
      inpDeviceCombo.setList(driverCombo.currDriver().recordingDevices);
    } else {
      outDeviceCombo.setList({});
      inpDeviceCombo.setList({});
    }
  }
};

DemoWidget::DemoWidget() : impl_{std::make_unique<Impl>()} {}

DemoWidget::~DemoWidget() {}

void DemoWidget::show() {
  ImGui::PushID(this);
  if (ImGui::Button("Refresh")) {
    impl_->updateDriverCombo();
    impl_->updateDeviceCombo();
  }

  static float refTextW{};
  if (refTextW < 1.0) {
    refTextW = ImGui::CalcTextSize("reference-text-for-calc-width").x;
  }

  // Driver Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(refTextW);
  if (impl_->driverCombo.show()) {
    impl_->updateDeviceCombo();
  }

  // Playback Device Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(refTextW * 2.f);
  impl_->outDeviceCombo.show();

  // Recording Device Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(refTextW * 2.f);
  impl_->inpDeviceCombo.show();

  ImGui::PopID();
}

} // namespace AudengiGui
} // namespace Mi
