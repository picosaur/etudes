#include "MiHttpWidget.h"
#include "EmHttpFetcher.h"
#include "MiImage.h"
#include <TextEditor.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_memory_editor.h>
#include <imgui_stdlib.h>

namespace Mi {
class HttpWidget::Impl {
public:
  std::string url{"https://a.tile.openstreetmap.org/0/0/0.png"};
  std::unique_ptr<Em::HttpFetcher> fetcher;
  TextEditor teditor;
  MemoryEditor meditor;

  Em::HttpHeaders respHeaders;
  int respMode{};
  std::string respStatus;
  std::unique_ptr<Image> respImage;
  std::string respText;

  Impl();
  void updateData();
  void showRequest();
  void showResponse();
  void showRespHeadersTable();
};

HttpWidget::HttpWidget() : impl_{std::make_unique<Impl>()} {}

HttpWidget::~HttpWidget() {}

void HttpWidget::show() {
  if (impl_->fetcher && impl_->fetcher->isDone()) {
    impl_->updateData();
    impl_->fetcher = {};
  }

  ImGui::BeginChild("##request", ImVec2(ImGui::GetWindowWidth() / 2.f, 0),
                    ImGuiChildFlags_Borders);
  ImGui::SeparatorText("Request");
  impl_->showRequest();
  ImGui::EndChild();
  ImGui::SameLine({}, 5.f);
  ImGui::BeginChild("##response", {}, ImGuiChildFlags_Borders);
  ImGui::SeparatorText("Response");
  impl_->showResponse();
  ImGui::EndChild();
}

HttpWidget::Impl::Impl() { teditor.SetReadOnly(true); }

void HttpWidget::Impl::updateData() {
  respHeaders = fetcher->responseHeaders();
  respStatus = fetcher->statusText();
  fetcher->assignData(respText);
  respImage = std::make_unique<Image>((const std::byte *)respText.data(),
                                      respText.size());
  teditor.SetText(respText);
}

void HttpWidget::Impl::showRequest() {
  ImGui::InputText("URL", &url);
  ImGui::SameLine();
  if (ImGui::Button("Fetch")) {
    respStatus = {};
    respText = {};
    fetcher = std::make_unique<Em::HttpFetcher>(url);
  }
}

void HttpWidget::Impl::showResponse() {
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Status");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(-1.f);
  ImGui::InputText("##", &respStatus);

  if (ImGui::CollapsingHeader("Headers")) {
    showRespHeadersTable();
  }

  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Display as");
  ImGui::SameLine();
  ImGui::RadioButton("None", &respMode, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Text", &respMode, 1);
  ImGui::SameLine();
  ImGui::RadioButton("Hex", &respMode, 2);
  ImGui::SameLine();
  ImGui::RadioButton("Image", &respMode, 3);

  if (respMode == 0) {

  } else if (respMode == 1) {
    teditor.Render("##");
  } else if (respMode == 2) {
    meditor.DrawContents(respText.data(), respText.size());
  } else if (respMode == 3 && respImage) {
    ImGui::Image(respImage->textureId(), respImage->size());
  }
}

void HttpWidget::Impl::showRespHeadersTable() {
  if (ImGui::BeginTable("HeadersTable", 2,
                        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("Value");
    ImGui::TableHeadersRow();
    for (const auto &header : respHeaders) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::PushID(&header);
      ImGui::PushID(&header.first);
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputText("##", (char *)header.first.data(), header.first.size(),
                       ImGuiInputTextFlags_ReadOnly);
      ImGui::PopID();
      ImGui::PushID(&header.second);
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputText("##", (char *)header.second.data(), header.second.size(),
                       ImGuiInputTextFlags_ReadOnly);
      ImGui::PopID();
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
}

} // namespace Mi
