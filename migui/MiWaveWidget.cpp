#include "MiWaveWidget.h"
#include "MiImage.h"
#include <implot.h>
#include <implot_internal.h>
#include <vector>

namespace Mi {

static constexpr double PI{3.14159265358979323846};
static constexpr double PI2{PI * 2.0};

class WaveWidget::Impl {
public:
  std::vector<double> y, ymin, ymax;
  float pxsx{};
  float sxpx{};
  std::unique_ptr<Image> image;
};

WaveWidget::WaveWidget() : impl_{std::make_unique<Impl>()} {
  double f{100};
  double dt = 1.0 / 48e3;
  double t{};
  int n{(int)1e5};
  for (int i{}; i < n; ++i) {
    t += dt;
    impl_->y.push_back(sin(PI2 * f * t));
  }
  impl_->ymax.resize(impl_->y.size());
  impl_->ymin.resize(impl_->y.size());
}

WaveWidget::~WaveWidget() {}

void WaveWidget::show() {
  if (ImPlot::BeginPlot("WavePlot", {-1, -100})) {
    const ImVec2 plotPos = ImPlot::GetPlotPos();
    const ImVec2 plotSize = ImPlot::GetPlotSize();
    const ImVec2 plotPosEnd = {plotPos.x + plotSize.x, plotPos.y + plotSize.y};
    const ImPlotRect plotLims = ImPlot::GetPlotLimits();

    if (ImPlot::BeginItem("Image")) {
      if (plotSize.x > 0 && plotSize.y > 0) {
        impl_->image =
            std::make_unique<Image>(plotSize.x / 32, plotSize.y / 32);
        impl_->image->fill(IM_COL32_WHITE);
        impl_->image->fillRow(10, IM_COL32_BLACK);
        impl_->image->fillCol(2, IM_COL32_BLACK, 0, 2);
        impl_->image->render();
      }
      if (impl_->image) {
        ImDrawList &draw_list = *ImPlot::GetPlotDrawList();
        draw_list.AddImage(impl_->image->textureId(), plotPos, plotPosEnd);
      }
    }

    ImPlot::PlotLine("Ref", impl_->y.data(), impl_->y.size());
    // ImPlot::PlotLine("Min", impl_->ymin.data(), impl_->ymin.size());
    // ImPlot::PlotLine("Max", impl_->ymax.data(), impl_->ymax.size());

    ImPlot::EndPlot();
  }
  ImGui::Text("Pixels per sample %.2f, Samples per pixel  %.2f", impl_->pxsx,
              impl_->sxpx);
}

// draw_list.AddLine(pmin, pmax, IM_COL32_WHITE, 1.f);
/*
      double ymin{999}, ymax{-999};
      int counter{};
      for (int i = 0; i < impl_->y.size(); ++i) {
        continue;
        if (i < smin.x) {
          continue;
        }
        if (i > smax.x) {
          break;
        }
        ymin = std::min(ymin, impl_->y[i]);
        ymax = std::max(ymax, impl_->y[i]);
        impl_->ymin[i] = ymin;
        impl_->ymax[i] = ymax;
        if (counter > sxpx * 4) {
          auto p1 = ImPlot::PlotToPixels(i, 0 IMPLOT_AUTO, IMPLOT_AUTO);
          auto p2 = ImPlot::PlotToPixels(i, ymax, IMPLOT_AUTO,
   IMPLOT_AUTO); draw_list.AddLine(p1, p2, IM_COL32_WHITE, sxpx * 4); ymin
   = 999; ymax = -999; counter = 0;
        }
        counter++;
      }

         ImPlot::EndItem();
     */

} // namespace Mi
