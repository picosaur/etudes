#include "FetchWidget.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include "EmFetcher.h"
#include "ImImage.h"

namespace Gui
{
    class FetchWidget::Impl
    {
    public:
        std::string url{"https://a.tile.openstreetmap.org/0/0/0.png"};
        std::unique_ptr<Em::Fetcher> fetcher;
        std::string status;
        std::unique_ptr<ImImage> image;
        std::string text;
    };

    FetchWidget::FetchWidget() : impl_{std::make_unique<Impl>()} {}

    FetchWidget::~FetchWidget() {}

    void FetchWidget::show() {
        ImGui::InputText("URL", &impl_->url);
        ImGui::SameLine();
        if (ImGui::Button("Fetch")) {
            impl_->status = {};
            impl_->text = {};
            impl_->image = {};
            impl_->fetcher = std::make_unique<Em::Fetcher>(impl_->url);
        }
        if (impl_->fetcher && impl_->fetcher->isDone()) {
            impl_->status = impl_->fetcher->statusText();
            impl_->fetcher->assign(impl_->text);
            impl_->image = std::make_unique<ImImage>(impl_->fetcher->data(), (int)impl_->fetcher->dataSize());
            impl_->fetcher = {};

        }
        ImGui::InputText("Status", &impl_->status);
        if (impl_->image) {
            ImGui::Image(impl_->image->textureId(), impl_->image->size());
        }
        ImGui::InputTextMultiline("Text", &impl_->text);
    }
}