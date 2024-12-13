#include "EtGuiWaveWidget.h"
#include "EtGuiImage.h"
#include "EtGuiWavePlot.h"
#include <implot.h>
#include <implot_internal.h>
#include <vector>

namespace EtGui {

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
  int n{(int)1e3};
  for (int i{}; i < n; ++i) {
    t += dt;
    impl_->y.push_back(sin(PI2 * f * t));
  }
  impl_->ymax.resize(impl_->y.size());
  impl_->ymin.resize(impl_->y.size());
}

WaveWidget::~WaveWidget() {}

void WaveWidget::show() {
  if (ImPlot::BeginPlot("Plot", {-1, -100})) {
    WavePlot::PlotPixelWave("##", impl_->y.data(), impl_->y.size());
    ImPlot::PlotLine("Ref", impl_->y.data(), impl_->y.size());
    //   ImPlot::PlotLine("Min", impl_->ymin.data(), impl_->ymin.size());
    //   ImPlot::PlotLine("Max", impl_->ymax.data(), impl_->ymax.size());

    ImPlot::EndPlot();
  }
}
} // namespace EtGui
