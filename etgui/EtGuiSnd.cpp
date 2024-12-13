#include "EtGuiSnd.h"
#include "EtSndManager.h"
#include <imgui.h>

namespace EtGui {
namespace SndGui {

// ============================================================================
// BASE IMPLEMENTATIONS
// ============================================================================

class RefSize {
  float baseWidth_{};

public:
  static RefSize &Get() {
    static RefSize rs;
    return rs;
  }

  float baseWidth(float coef = 1.f) {
    if (baseWidth_ < 1.0) {
      baseWidth_ = ImGui::CalcTextSize("reference-text-for-calc-width").x;
    }
    return baseWidth_ * coef;
  }
};

// DriverDeviceComboImpl
// ----------------------------------------------------------------------------
template <typename T> class DriverDeviceComboImpl {
public:
  std::string label{"Combo"};
  T list{};
  std::string curr{};

  DriverDeviceComboImpl(const std::string &label_) : label{label_} {}

  bool show() {
    bool changed{};
    ImGui::PushID(this);
    if (ImGui::BeginCombo(label.c_str(), curr.c_str())) {
      for (const auto &item : list) {
        if (ImGui::Selectable(item.name.c_str(), item.name == curr)) {
          curr = item.name;
          changed = true;
        }
      }
      ImGui::EndCombo();
    }
    ImGui::PopID();
    return changed;
  }

  void setList(const T &list_) {
    list = list_;
    if (list.empty()) {
      curr = {};
    } else if (!list.contains(curr)) {
      curr = list.first().name;
    }
  }

  void setCurr(const std::string &name_) {
    if (!name_.empty() && !list.contains(name_)) {
      throw(
          std::runtime_error("DriverDeviceComboImpl::setCurr list_.contains"));
    }
    curr = name_;
  }

  bool hasCurr() const { return !curr.empty(); }

  const auto &currItem() const { return list.at(curr); }
};

// IntVecComboImpl
// ----------------------------------------------------------------------------
class IntVecComboImpl {
public:
  std::string label{"Combo"};
  std::vector<std::string> list{};
  std::string curr{};

  void setCurr(int curr_) { curr = std::to_string(curr_); }

  void setList(const std::vector<int> &list_) {
    for (const auto &item : list_) {
      list.push_back(std::to_string(item));
    }
    if (!list.empty()) {
      curr = list.front();
    } else {
      curr = {};
    }
  }

  bool show() {
    bool changed{};
    ImGui::PushID(this);
    if (ImGui::BeginCombo(label.c_str(), curr.c_str())) {
      for (const auto &item : list) {
        if (ImGui::Selectable(item.c_str(), item == curr)) {
          curr = item;
          changed = true;
        }
      }
      ImGui::EndCombo();
    }
    ImGui::PopID();
    return changed;
  }
};

// IntSpinImpl
// ----------------------------------------------------------------------------
class IntSpinImpl {
public:
  std::string label{"Spin"};
  int min{};
  int max{99};
  int value{};
  int step{1};
  int stepFast{10};
  ImGuiInputTextFlags flags{};

  bool show() {
    bool changed = {};
    int curr = value;
    ImGui::PushID(this);
    if (ImGui::InputInt(label.c_str(), &value, step, stepFast, flags)) {
      value = std::clamp(value, min, max);
      if (value != curr) {
        changed = true;
      }
    }
    ImGui::PopID();
    return changed;
  }
};

// ============================================================================
// SPECIFIC ELEMENTS
// ============================================================================

// DriverCombo
// ----------------------------------------------------------------------------
class DriverCombo::Impl : public DriverDeviceComboImpl<EtSnd::DriverList> {
public:
  Impl(const std::string &label)
      : DriverDeviceComboImpl<EtSnd::DriverList>{label} {}
};

DriverCombo::DriverCombo(const std::string &label)
    : impl_{std::make_unique<Impl>(label)} {}

DriverCombo::~DriverCombo() {}

bool DriverCombo::show() { return impl_->show(); }

void DriverCombo::setList(const EtSnd::DriverList &list) {
  impl_->setList(list);
}

void DriverCombo::setCurr(const std::string &name) { impl_->setCurr(name); }

bool DriverCombo::hasCurr() const { return impl_->hasCurr(); }

const EtSnd::DriverInfo &DriverCombo::curr() const { return impl_->currItem(); }

// DeviceCombo
// ----------------------------------------------------------------------------
class DeviceCombo::Impl : public DriverDeviceComboImpl<EtSnd::DeviceList> {
public:
  Impl(const std::string &label)
      : DriverDeviceComboImpl<EtSnd::DeviceList>{label} {}
};

DeviceCombo::DeviceCombo(const std::string &label)
    : impl_{std::make_unique<Impl>(label)} {}

DeviceCombo::~DeviceCombo() {}

bool DeviceCombo::show() { return impl_->show(); }

void DeviceCombo::setList(const EtSnd::DeviceList &list) {
  impl_->setList(list);
}

void DeviceCombo::setCurr(const std::string &name) { impl_->setCurr(name); }

bool DeviceCombo::hasCurr() const { return impl_->hasCurr(); }

const EtSnd::DeviceInfo &DeviceCombo::curr() const { return impl_->currItem(); }

// SampleRateCombo
// ----------------------------------------------------------------------------
class SampleRateCombo::Impl : public IntVecComboImpl {};

SampleRateCombo::SampleRateCombo(const std::string &label,
                                 const std::vector<int> &list, int curr)
    : impl_{std::make_unique<Impl>()} {
  impl_->label = label;
  impl_->setList(list);
  impl_->setCurr(curr);
}

SampleRateCombo::~SampleRateCombo() {}

bool SampleRateCombo::show() { return impl_->show(); }

// BufferCombo
// ----------------------------------------------------------------------------
class BufferSizeCombo::Impl : public IntVecComboImpl {};

BufferSizeCombo::BufferSizeCombo(const std::string &label,
                                 const std::vector<int> &list, int curr)
    : impl_{std::make_unique<Impl>()} {
  impl_->label = label;
  impl_->setList(list);
  impl_->setCurr(curr);
}

BufferSizeCombo::~BufferSizeCombo() {}

bool BufferSizeCombo::show() { return impl_->show(); }

// ChannelsSpin
// ----------------------------------------------------------------------------
class ChannelsSpin::Impl : public IntSpinImpl {};

ChannelsSpin::ChannelsSpin(const std::string &label, int min, int max)
    : impl_{std::make_unique<Impl>()} {
  impl_->label = label;
  impl_->min = min;
  impl_->max = max;
  impl_->value = min;
  impl_->label = label;
}

ChannelsSpin::~ChannelsSpin() {}

bool ChannelsSpin::show() { return impl_->show(); }

// ============================================================================
// WIDGETS
// ============================================================================

// DeviceChooserWidget
// ----------------------------------------------------------------------------
class DeviceChooserWidget::Impl {
public:
  DriverCombo driverCombo;
  DeviceCombo captDeviceCombo{"Capture"}, playDeviceCombo{"Playback"};

  void updateDriverCombo();
  void updateDeviceCombo();
  void refreshDrivers();
};

DeviceChooserWidget::DeviceChooserWidget() : impl_{std::make_unique<Impl>()} {
  impl_->refreshDrivers();
}

DeviceChooserWidget::~DeviceChooserWidget() {}

void DeviceChooserWidget::show() {
  ImGui::PushID(this);

  // Label
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Device");

  // Refresh
  ImGui::SameLine();
  if (ImGui::Button("Refresh")) {
    impl_->refreshDrivers();
  }

  // Driver Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(RefSize::Get().baseWidth());
  if (impl_->driverCombo.show()) {
    impl_->updateDeviceCombo();
  }

  // Playback Device Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(RefSize::Get().baseWidth(2));
  impl_->playDeviceCombo.show();

  // Recording Device Combo
  ImGui::SameLine();
  ImGui::SetNextItemWidth(RefSize::Get().baseWidth(2));
  impl_->captDeviceCombo.show();

  ImGui::PopID();
}

void DeviceChooserWidget::Impl::updateDriverCombo() {
  auto &man = EtSnd::Manager::Get();
  man.discoverDrivers();
  driverCombo.setList(man.drivers());
  driverCombo.setCurr(man.defaultDriver().name);
}

void DeviceChooserWidget::Impl::updateDeviceCombo() {
  if (driverCombo.hasCurr()) {
    // playback
    playDeviceCombo.setList(driverCombo.curr().playbackDevices);
    playDeviceCombo.setCurr(driverCombo.curr().defaultPlaybackDevice.name);
    // capture
    captDeviceCombo.setList(driverCombo.curr().captureDevices);
    captDeviceCombo.setCurr(driverCombo.curr().defaultCaptureDevice.name);
  } else {
    playDeviceCombo.setList({});
    captDeviceCombo.setList({});
  }
}

void DeviceChooserWidget::Impl::refreshDrivers() {
  updateDriverCombo();
  updateDeviceCombo();
}

// StreamSetupWidget
// ----------------------------------------------------------------------------
class StreamSetupWidget::Impl {
public:
  SampleRateCombo sampleRateCombo;
  BufferSizeCombo bufferSizeCombo;
  ChannelsSpin channelsSpin;
};

StreamSetupWidget::StreamSetupWidget() : impl_{std::make_unique<Impl>()} {}

StreamSetupWidget::~StreamSetupWidget() {}

void StreamSetupWidget::show() {
  // Label
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Stream");

  // Sample Rate
  ImGui::SameLine();
  ImGui::SetNextItemWidth(RefSize::Get().baseWidth());
  impl_->sampleRateCombo.show();

  // Buffer Size
  ImGui::SameLine();
  ImGui::SetNextItemWidth(RefSize::Get().baseWidth());
  impl_->bufferSizeCombo.show();

  // Channels
  ImGui::SameLine();
  ImGui::SetNextItemWidth(RefSize::Get().baseWidth());
  impl_->channelsSpin.show();
}

// ============================================================================
// DEMO
// ============================================================================

class DemoWidget::Impl {
public:
  DeviceChooserWidget deviceChooserWidget;
  StreamSetupWidget streamSetupWidget;
};

DemoWidget::DemoWidget() : impl_{std::make_unique<Impl>()} {}

DemoWidget::~DemoWidget() {}

void DemoWidget::show() {
  impl_->deviceChooserWidget.show();
  impl_->streamSetupWidget.show();
  ImGui::SeparatorText("Oscillator");
  if (ImGui::Button("Start")) {
  }
}

} // namespace SndGui
} // namespace EtGui
