#include "MiHttpFetchWidget.h"
#include "EmHttpFetcher.h"
#include "MiImage.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

namespace Mi {
class HttpFetchWidget::Impl {
public:
  std::string url{"https://a.tile.openstreetmap.org/0/0/0.png"};
  std::unique_ptr<Em::HttpFetcher> fetcher;
  std::string status;
  std::unique_ptr<Image> image;
  std::string text;
};

HttpFetchWidget::HttpFetchWidget() : impl_{std::make_unique<Impl>()} {}

HttpFetchWidget::~HttpFetchWidget() {}

void HttpFetchWidget::show() {
  ImGui::InputText("URL", &impl_->url);
  ImGui::SameLine();
  if (ImGui::Button("Fetch")) {
    impl_->status = {};
    impl_->text = {};
    impl_->image = {};
    impl_->fetcher = std::make_unique<Em::HttpFetcher>(impl_->url);
  }
  if (impl_->fetcher && impl_->fetcher->isDone()) {
    impl_->status = impl_->fetcher->statusText();
    // impl_->fetcher->assignData(impl_->text);
    impl_->image = std::make_unique<Image>(impl_->fetcher->data(),
                                           (int)impl_->fetcher->dataSize());
    impl_->text.clear();
    const auto headers{impl_->fetcher->responseHeaders()};
    for (const auto &header : headers) {
      impl_->text.append(header.first);
      impl_->text.append("\t");
      impl_->text.append(header.second);
      impl_->text.append("\n");
    }

    impl_->fetcher = {};
  }
  ImGui::InputText("Status", &impl_->status);
  if (impl_->image) {
    ImGui::Image(impl_->image->textureId(), impl_->image->size());
  }
  ImGui::InputTextMultiline("Text", &impl_->text);
  ImGui::Separator();
  ImGui::InputTextMultiline("Text2", &impl_->text);
}
} // namespace Mi