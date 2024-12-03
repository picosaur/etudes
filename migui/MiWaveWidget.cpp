#include "MiWaveWidget.h"
#include <implot.h>
#include <implot_internal.h>
#include <vector>

namespace Mi {

static constexpr double PI{3.14159265358979323846};
static constexpr double PI2{PI * 2.0};

class WaveWidget::Impl {
public:
  std::vector<double> y;
  float pxsx{};
  float sxpx{};
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
}

WaveWidget::~WaveWidget() {}

void WaveWidget::show() {
  if (ImPlot::BeginPlot("WavePlot", {-1, -100})) {
    // sample
    auto slims = ImPlot::GetPlotLimits(ImAxis_X1, ImAxis_Y1);
    auto smin = slims.Min();
    auto smax = slims.Max();
    auto sxrange = smax.x - smin.x;

    // pixel
    auto pmin = ImPlot::PlotToPixels(slims.X.Min, slims.Y.Min, IMPLOT_AUTO,
                                     IMPLOT_AUTO);
    auto pmax = ImPlot::PlotToPixels(slims.X.Max, slims.Y.Max, IMPLOT_AUTO,
                                     IMPLOT_AUTO);

    auto pxrange = pmax.x - pmin.x;
    auto pxsx = pxrange / sxrange; // pixels per sample
    auto sxpx = sxrange / pxrange; // samples per pixel

    if (ImPlot::BeginItem("Wave")) {
      ImDrawList &draw_list = *ImPlot::GetPlotDrawList();

      draw_list.AddLine(pmin, pmax, IM_COL32_WHITE, 1.f);

      /*
      ImVec2 p1, p2;
      for (auto i{0}; i < impl_->y.size(); ++i) {
        if (i < 0 || i >= impl_->y.size()) {
          continue;
        }
        ImPlot::PushPlotClipRect();
        draw_list.AddLine({pmin.x + (float)i, 0.f}, {pmin.x + (float)i, 1.f},
                          IM_COL32_WHITE, 1.f);
        ImPlot::PopPlotClipRect();
      }
  */

      ImPlot::EndItem();
    }

    ImPlot::PlotLine("Ref", impl_->y.data(), impl_->y.size());

    impl_->pxsx = pxsx;
    impl_->sxpx = sxpx;

    ImPlot::EndPlot();
  }
  ImGui::Text("Pixels per sample %.2f, Samples per pixel  %.2f", impl_->pxsx,
              impl_->sxpx);
}

} // namespace Mi
