#include "EtGuiFire.h"
#include "EtFireManager.h"
#include <imgui.h>
#include <string>
#include <vector>

namespace EtGuiw {
namespace FireGui {

class DemoWidget::Impl {
public:
  std::vector<EtFire::BackendInfo> backends;
};

DemoWidget::DemoWidget() : impl_{std::make_unique<Impl>()} {
  impl_->backends = EtFire::Manager::Get().backends();
}

DemoWidget::~DemoWidget() {}

void DemoWidget::show() {
  if (ImGui::BeginListBox("Backends")) {
    for (const auto &item : impl_->backends) {
      ImGui::Selectable(item.name.c_str());
    }
    ImGui::EndListBox();
  }

  if (ImGui::BeginTable("Devices", 5)) {
    ImGui::TableSetupColumn("Backend");
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Platform");
    ImGui::TableSetupColumn("Toolkit");
    ImGui::TableSetupColumn("Compute");
    ImGui::TableHeadersRow();
    for (const auto &back : impl_->backends) {
      for (const auto &dev : back.devices) {
        ImGui::TableNextRow();
        // 0 backend
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(back.name.c_str());
        // 1 name
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(dev.name.c_str());
        // 2 platform
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(dev.platform.c_str());
        // 3 toolkit
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(dev.toolkit.c_str());
        // 4 compute
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(dev.compute.c_str());
      }
    }

    ImGui::EndTable();
  }
}

} // namespace FireGui
} // namespace EtGuiw
