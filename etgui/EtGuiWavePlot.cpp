#include "EtGuiWavePlot.h"
#include "EtGuiColor.h"
#include "EtGuiContext.h"
#include "EtGuiImage.h"
#include "EtGuiPopup.h"
#include <algorithm>
#include <implot.h>
#include <implot_internal.h>
#include <limits>
#include <vector>

namespace EtGui {
namespace WavePlot {

// Pixel Wave
// ----------------------------------------------------------------------------
class PixelWaveData {
public:
  std::unique_ptr<Image> image;
};

using PixelWaveContext = Context<PixelWaveData>;

void PlotPixelWave(const char *label, double *ys, int count) {
  auto ctx = PixelWaveContext::Get()->pool.GetOrAddByKey(ImGui::GetID(label));

  const ImVec2 plotPos = ImPlot::GetPlotPos();
  const ImVec2 plotSize = ImPlot::GetPlotSize();
  const ImVec2 plotPosEnd = {plotPos.x + plotSize.x, plotPos.y + plotSize.y};
  const ImPlotRect plotLims = ImPlot::GetPlotLimits();

  if (ImPlot::BeginItem(label)) {
    if (plotSize.x > 0 && plotSize.y > 0) {
      ctx->image = std::make_unique<Image>(plotSize.x / 4, plotSize.y / 4);
      ctx->image->fill(Color::Transparent);
      // border
      ctx->image->fillRect(Color::Vga::BrightRed);
      // test
      ctx->image->fillRow(10, Color::Vga::BrightRed, 10, 10);
      // wave
      DrawWaveOnImage(ctx->image.get(), ys, count, plotLims,
                      Color::Vga::BrightYellow);

      // render
      ctx->image->render();
    }
    if (ctx->image) {

      ImDrawList &draw_list = *ImPlot::GetPlotDrawList();
      draw_list.AddImage(ctx->image->textureId(), plotPos, plotPosEnd, {0, 1},
                         {1, 0});
    }
    ImPlot::EndItem();
  }

  if (Popup::BeginCentered("##")) {
    ImGui::Text("Plot Lims X %.2f", plotLims.X.Size());
    ImGui::End();
  }
}

void DrawWaveOnImage(Image *image, double *ys, int count,
                     const ImPlotRect &plotLims, ImU32 color) {
  const double loVal = {std::numeric_limits<double>::lowest()};
  const double hiVal = {std::numeric_limits<double>::max()};

  const double dx = plotLims.X.Size() / double(image->width());
  const double dy = plotLims.Y.Size() / double(image->height());

  double hi{loVal};
  double lo{hiVal};

  double step = 1.0;

  double xx{};
  int j0{};
  int row0{}, row1{};
  int col0{}, col1{};

  for (int i{}; i < count; ++i) {
    lo = std::min(lo, ys[i]);
    hi = std::max(hi, ys[i]);
    if (xx >= dx) {
      col0 = col1;
      col1 = int((i * step - plotLims.X.Min) / dx);
      row0 = (lo - plotLims.Y.Min) / dy - 2;
      row1 = (hi - plotLims.Y.Min) / dy + 2;
      row0 = std::clamp(row0, 0, image->height() - 1);
      row1 = std::clamp(row1, 0, image->height() - 1);
      if (col1 >= 0 && col1 < image->width()) {
        image->fillCol(col1, color, row0, row1);
      }
      lo = hiVal;
      hi = loVal;
      xx = {};
    }
    xx += step;
  }
}

// Shaded Wave
// ----------------------------------------------------------------------------
class ShadedWaveData {
public:
  std::vector<double> x, yhi, ylo;

  void setData(double *ys, int count, const ImPlotRect &plotLims, int width,
               int height);
};

using ShadedWaveContext = Context<ShadedWaveData>;

void ShadedWaveData::setData(double *ys, int count, const ImPlotRect &plotLims,
                             int width, int height) {
  x.clear();
  yhi.clear();
  ylo.clear();

  const double loVal = {std::numeric_limits<double>::lowest()};
  const double hiVal = {std::numeric_limits<double>::max()};

  const double dx = plotLims.X.Size() / double(width);
  const double dy = plotLims.Y.Size() / double(height);

  double step = 1.0;
  double hi{loVal};
  double lo{hiVal};
  double xx{};

  for (int i{}; i < count; ++i) {
    lo = std::min(lo, ys[i]);
    hi = std::max(hi, ys[i]);
    if (xx >= dx) {
      x.push_back(i);
      ylo.push_back(lo);
      yhi.push_back(hi);
      lo = hiVal;
      hi = loVal;
      xx = {};
    }
    xx += step;
  }
}

void PlotShadedWave(const char *label, double *ys, int count) {
  auto ctx = ShadedWaveContext::Get()->pool.GetOrAddByKey(ImGui::GetID(label));

  const ImVec2 plotPos = ImPlot::GetPlotPos();
  const ImVec2 plotSize = ImPlot::GetPlotSize();
  const ImVec2 plotPosEnd = {plotPos.x + plotSize.x, plotPos.y + plotSize.y};
  const ImPlotRect plotLims = ImPlot::GetPlotLimits();

  ctx->setData(ys, count, plotLims, plotSize.x / 16, plotSize.y / 16);

  ImPlot::PlotShaded("##", ctx->x.data(), ctx->ylo.data(), ctx->yhi.data(),
                     ctx->x.size());
}

} // namespace WavePlot
} // namespace EtGui
