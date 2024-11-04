#include "FetchWidget.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include "EmFetcher.h"

namespace gui
{
    class FetchWidget::Impl
    {
    public:
        std::string url{"https://emscripten.org/docs/api_reference/fetch.html"};
        std::unique_ptr<Em::Fetcher> fetcher;
        std::string data;
    };

    FetchWidget::FetchWidget() : impl_{std::make_unique<Impl>()} {}

    FetchWidget::~FetchWidget() {}

    void FetchWidget::show() {
        ImGui::InputText("URL", &impl_->url);
        ImGui::SameLine();
        if (ImGui::Button("Fetch")) {
            impl_->fetcher = std::make_unique<Em::Fetcher>(impl_->url);
        }
        if (impl_->fetcher && impl_->fetcher->isDone()) {
            impl_->data = {};
            impl_->data.assign(impl_->fetcher->data(), impl_->fetcher->data() + impl_->fetcher->dataSize());
            impl_->fetcher = {};
        }
        ImGui::InputTextMultiline("Data", &impl_->data);
    }
}