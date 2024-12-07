#include "MiHttpWidget.h"
#include "EmHttp.h"
#include "EmHttpFetcher.h"
#include "MiImage.h"
#include <TextEditor.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_memory_editor.h>
#include <imgui_stdlib.h>
#include <imgui_tex_inspect.h>

namespace Mi {

class HttpHeadersList {
public:
  class Item {
  public:
    std::string first{};
    std::string second{};
    bool enabled{};
  };

  HttpHeadersList() {}

  HttpHeadersList(std::size_t n) { list_.resize(n); }

  HttpHeadersList(const Em::HttpHeaders &emHeaders) {
    list_.resize(emHeaders.size());
    setHeaders(emHeaders);
  }

  auto begin() { return list_.begin(); }
  auto end() { return list_.end(); }

  auto begin() const { return list_.begin(); }
  auto end() const { return list_.end(); }

  void setHeaders(const Em::HttpHeaders &emHeaders) {
    std::for_each(list_.begin(), list_.end(), [](auto &item) { item = {}; });
    std::size_t i{};
    for (const auto &header : emHeaders) {
      if (i >= list_.size()) {
        break;
      }
      list_.at(i++) = {header.first, header.second};
    }
    std::for_each(list_.begin(), list_.end(),
                  [](auto &item) { item.enabled = !item.second.empty(); });
    std::sort(list_.begin(), list_.begin() + i,
              [](const auto &a, const auto &b) { return a.first < b.first; });
  }

  void nemptyEnable() {
    std::for_each(list_.begin(), list_.end(),
                  [](auto &item) { item.enabled = !item.second.empty(); });
  }

  void allSetEnabled(bool v) {
    std::for_each(list_.begin(), list_.end(),
                  [&](auto &item) { item.enabled = v; });
  }

  void allClear() {
    std::for_each(list_.begin(), list_.end(), [](auto &item) { item = {}; });
  }

private:
  std::vector<Item> list_{};
};

class HttpResponse {
public:
  HttpHeadersList headers;
  std::string status;
  std::string text;
  std::unique_ptr<Image> image;
};

class HttpWidget::Impl {
public:
  std::string url{"https://a.tile.openstreetmap.org/0/0/0.png"};
  std::unique_ptr<Em::HttpFetcher> fetcher;
  TextEditor teditor;
  MemoryEditor meditor;

  HttpHeadersList reqHeaders{99};
  HttpResponse resp;
  int respDisplMode{};

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
  setBrowserReqHeaders(url);
}

void HttpWidget::Impl::updateData() {
  resp.headers = fetcher->responseHeaders();
  resp.status = fetcher->statusText();
  fetcher->assignData(resp.text);
  resp.image = std::make_unique<Image>((const std::byte *)resp.text.data(),
                                       resp.text.size());
  teditor.SetText(resp.text);
}

void HttpWidget::Impl::showRequest() {
  ImGui::InputText("URL", &url);
  ImGui::SameLine();
  if (ImGui::Button("Fetch")) {
    resp = {};
    fetcher = std::make_unique<Em::HttpFetcher>(url, getReqHeaders());
  }
  showReqHeadersTable();
}

void HttpWidget::Impl::showResponse() {
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Status");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(-1.f);
  ImGui::InputText("##", &resp.status);

  if (ImGui::CollapsingHeader("Headers")) {
    showRespHeadersTable();
  }

  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Display As");
  ImGui::SameLine();
  ImGui::RadioButton("None", &respDisplMode, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Text", &respDisplMode, 1);
  ImGui::SameLine();
  ImGui::RadioButton("Hex", &respDisplMode, 2);
  ImGui::SameLine();
  ImGui::RadioButton("Image", &respDisplMode, 3);

  if (respDisplMode == 0) {

  } else if (respDisplMode == 1) {
    teditor.Render("##");
  } else if (respDisplMode == 2) {
    meditor.DrawContents(resp.text.data(), resp.text.size());
  } else if (respDisplMode == 3 && resp.image) {
    // ImGui::Image(resp.image->textureId(), resp.image->imageSize());
    ImGuiTexInspect::BeginInspectorPanel("##asd", resp.image->textureId(),
                                         resp.image->imageSize());
    ImGuiTexInspect::EndInspectorPanel();
  }
}

void HttpWidget::Impl::showRespHeadersTable() {
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
  if (ImGui::BeginTable("ResponseHeadersTable", 3, ImGuiTableFlags_Borders)) {
    static float keyColW{};
    if (keyColW < 1.f) {
      keyColW = ImGui::CalcTextSize("reference-text-used-for-calc-width").x;
    }
    ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, keyColW);
    ImGui::TableSetupColumn("Value");
    ImGui::TableHeadersRow();
    for (const auto &header : resp.headers) {
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
    reqHeaders.allClear();
  }
  ImGui::SameLine();
  if (ImGui::Button("Enable ALL")) {
    reqHeaders.allSetEnabled(true);
  }
  ImGui::SameLine();
  if (ImGui::Button("Enable N-E")) {
    reqHeaders.nemptyEnable();
  }
  ImGui::SameLine();
  if (ImGui::Button("Disable ALL")) {
    reqHeaders.allSetEnabled(false);
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
  if (ImGui::BeginTable("RequestHeadersTable", 4, ImGuiTableFlags_Borders)) {
    static float keyColWidth{};
    if (keyColWidth < 1.0) {
      keyColWidth = ImGui::CalcTextSize("reference-text-used-for-calc-width").x;
    }
    ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Enable", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed,
                            keyColWidth);
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

  reqHeaders.setHeaders(
      Em::Http::BrowserRequestHeaders(Em::Http::UrlHost(url)));
}

void HttpWidget::Impl::setStandardReqHeaders() {
  reqHeaders.setHeaders(Em::Http::StandardRequestHeaders());
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
