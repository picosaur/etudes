#include "MiHttpWidget.h"
#include "EmHttp.h"
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

  class ReqHeaderItem {
  public:
    std::string first{};
    std::string second{};
    bool enabled{};
  };
  std::vector<ReqHeaderItem> reqHeaders{256};

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
  void showReqHeadersTable();
  void setStandardReqHeaders();
  void setBrowserReqHeaders(const std::string &url);
  Em::HttpHeaders getReqHeaders() const;
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

HttpWidget::Impl::Impl() {
  teditor.SetReadOnly(true);
  std::for_each(reqHeaders.begin(), reqHeaders.end(),
                [](auto &item) { item = {}; });
}

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
    fetcher = std::make_unique<Em::HttpFetcher>(url, getReqHeaders());
  }
  showReqHeadersTable();
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
  ImGui::TextUnformatted("Display As");
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
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
  if (ImGui::BeginTable("ResponseHeadersTable", 3, ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("Value");
    ImGui::TableHeadersRow();
    for (const auto &header : respHeaders) {
      // row
      ImGui::PushID(&header);
      ImGui::TableNextRow();
      // index
      ImGui::TableNextColumn();
      ImGui::AlignTextToFramePadding();
      ImGui::Text("%.d", ImGui::TableGetRowIndex());
      // key
      ImGui::PushID(&header.first);
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputText("##", (char *)header.first.data(), header.first.size(),
                       ImGuiInputTextFlags_ReadOnly);
      ImGui::PopID();
      // value
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
  ImGui::PopStyleVar();
}

void HttpWidget::Impl::showReqHeadersTable() {
  //
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Built-in Headers Sets");
  ImGui::SameLine();
  if (ImGui::Button("Standard")) {
    setStandardReqHeaders();
  }
  ImGui::SameLine();
  if (ImGui::Button("Browser")) {
    setBrowserReqHeaders(url);
  }

  //
  if (ImGui::Button("Clear")) {
    std::for_each(reqHeaders.begin(), reqHeaders.end(),
                  [](auto &item) { item = {}; });
  }
  ImGui::SameLine();
  if (ImGui::Button("Enable ALL")) {
    std::for_each(reqHeaders.begin(), reqHeaders.end(),
                  [](auto &item) { item.enabled = true; });
  }
  ImGui::SameLine();
  if (ImGui::Button("Enable N-E")) {
    std::for_each(reqHeaders.begin(), reqHeaders.end(),
                  [](auto &item) { item.enabled = !item.second.empty(); });
  }
  ImGui::SameLine();
  if (ImGui::Button("Disable ALL")) {
    std::for_each(reqHeaders.begin(), reqHeaders.end(),
                  [](auto &item) { item.enabled = {}; });
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
  if (ImGui::BeginTable("RequestHeadersTable", 4, ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Enable", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Key");
    ImGui::TableSetupColumn("Value");
    ImGui::TableHeadersRow();
    for (auto &header : reqHeaders) {
      // row
      ImGui::PushID(&header);
      ImGui::TableNextRow();
      // index
      ImGui::TableNextColumn();
      ImGui::AlignTextToFramePadding();
      ImGui::Text("%.d", ImGui::TableGetRowIndex());
      // flag
      ImGui::PushID(&header.enabled);
      ImGui::TableNextColumn();
      ImGui::Checkbox("##", &header.enabled);
      ImGui::PopID();
      // key
      ImGui::PushID(&header.first);
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputText("##", &header.first);
      ImGui::PopID();
      // value
      ImGui::PushID(&header.second);
      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputText("##", &header.second);
      ImGui::PopID();
      ImGui::PopID();
    }

    ImGui::EndTable();
  }
  ImGui::PopStyleVar();
}

void HttpWidget::Impl::setBrowserReqHeaders(const std::string &url) {
  std::for_each(reqHeaders.begin(), reqHeaders.end(),
                [](auto &item) { item = {}; });

  auto emHeaders = Em::Http::GetBrowserRequestHeaders(Em::Http::UrlToHost(url));
  std::size_t i{};
  for (const auto &header : emHeaders) {
    reqHeaders.at(i++) = {header.first, header.second};
  }

  std::for_each(reqHeaders.begin(), reqHeaders.end(),
                [](auto &item) { item.enabled = !item.second.empty(); });
}

void HttpWidget::Impl::setStandardReqHeaders() {
  std::for_each(reqHeaders.begin(), reqHeaders.end(),
                [](auto &item) { item = {}; });
  auto emHeaders = Em::Http::GetStandardHeaders();
  std::size_t i{};
  for (const auto &header : emHeaders) {
    reqHeaders.at(i++) = {header.first, header.second};
  }
  std::for_each(reqHeaders.begin(), reqHeaders.end(),
                [](auto &item) { item.enabled = !item.second.empty(); });
}

Em::HttpHeaders HttpWidget::Impl::getReqHeaders() const {
  Em::HttpHeaders emHeaders;
  for (const auto &header : reqHeaders) {
    if (!header.enabled) {
      continue;
    }
    if (header.first.empty()) {
      continue;
    }
    emHeaders.insert({header.first, header.second});
  }
  return emHeaders;
}

} // namespace Mi
