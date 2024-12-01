#include "MiAudengiWidget.h"
#include "Audengi.h"
#include <imgui.h>

namespace Mi {

class AudengiWidget::Impl {
public:
  std::vector<std::string> drivers;
  std::vector<Audengi::DeviceInfo> inpDevices, outDevices;
};

AudengiWidget::AudengiWidget() : impl_{std::make_unique<Impl>()} {}

AudengiWidget::~AudengiWidget() {}

void AudengiWidget::show() {
  if (ImGui::Button("Discover")) {
    impl_->drivers = Audengi::Engine::Get().audioDrivers();
  }

  if (ImGui::BeginTable("##", 1)) {
    for (const auto &drv : impl_->drivers) {
      ImGui::TableNextRow();
      //
      ImGui::TableNextColumn();
      ImGui::TextUnformatted(drv.c_str());
    }
    ImGui::EndTable();
  }
}

} // namespace Mi
