#include "MiAudengiGui.h"
#include "Audengi.h"
#include <algorithm>
#include <imgui.h>

namespace Mi {
namespace AudengiGui {

// DriverCombo
// ----------------------------------------------------------------------------
class DriverCombo::Impl {
public:
  Audengi::DriverList list;
  std::string curr;

  bool currInList() const { return false; }
};

DriverCombo::DriverCombo() : impl_{std::make_unique<Impl>()} {}

DriverCombo::~DriverCombo() {}

void DriverCombo::show() {
  ImGui::PushID(this);
  if (ImGui::BeginCombo("Driver", impl_->curr.c_str())) {
    for (const auto &item : impl_->list) {
      if (ImGui::Selectable(item.name.c_str(), item.name == impl_->curr)) {
        impl_->curr = item.name;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
}

void DriverCombo::setList(const Audengi::DriverList &list) {
  impl_->list = list;
  if (!impl_->currInList()) {
    if (impl_->list.empty()) {
      impl_->curr = {};
    }
  }
}

// DeviceCombo
// ----------------------------------------------------------------------------
class DeviceCombo::Impl {
public:
  std::vector<Audengi::DeviceInfo> list;
  std::string curr;

  bool currInList() const {
    /*
    return std::find(list.begin(), list.end(), [this](const auto &item) {
             return item.name == curr;
           }) != list.end();
*/
    return false;
  }
};

DeviceCombo::DeviceCombo() : impl_{std::make_unique<Impl>()} {}

DeviceCombo::~DeviceCombo() {}

void DeviceCombo::show() {
  ImGui::PushID(this);
  if (ImGui::BeginCombo("Device", impl_->curr.c_str())) {
    for (const auto &item : impl_->list) {
      if (ImGui::Selectable(item.name.c_str(), item.name == impl_->curr)) {
        impl_->curr = item.name;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
}

void DeviceCombo::update() {
  if (!impl_->currInList()) {
    if (impl_->list.empty()) {
      impl_->curr = {};
    }
  }
}

// DemoWidget
// ----------------------------------------------------------------------------
class DemoWidget::Impl {
public:
  DriverCombo driverCombo;
  DeviceCombo deviceCombo;
  std::vector<Audengi::DeviceInfo> inpDevices, outDevices;
};

DemoWidget::DemoWidget() : impl_{std::make_unique<Impl>()} {}

DemoWidget::~DemoWidget() {}

void DemoWidget::show() {
  ImGui::PushID(this);
  if (ImGui::Button("Refresh")) {
    Audengi::Manager::Get().discoverAndTest();
  }

  static float refTextW{};
  if (refTextW < 1.0) {
    refTextW = ImGui::CalcTextSize("reference-text-for-calc-width").x;
  }

  // Driver Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(refTextW);
  impl_->driverCombo.show();

  // Device Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(refTextW);
  impl_->deviceCombo.show();

  ImGui::PopID();
}

} // namespace AudengiGui
} // namespace Mi
