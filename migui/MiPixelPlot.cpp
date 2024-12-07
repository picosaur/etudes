#include "MiPixelPlot.h"
#include "MiColor.h"
#include "MiContext.h"
#include "MiImage.h"
#include "MiPopup.h"
#include <algorithm>
#include <implot.h>
#include <implot_internal.h>
#include <limits>
#include <vector>

namespace Mi {
namespace PixelPlot {

class WaveData {
public:
  std::unique_ptr<Image> image;
  std::vector<double> x, yhi, ylo;
};

using WaveContext = Context<WaveData>;

void PlotPixelWave(const char *label, double *ys, int count) {
  const ImVec2 plotPos = ImPlot::GetPlotPos();
  const ImVec2 plotSize = ImPlot::GetPlotSize();
  const ImVec2 plotPosEnd = {plotPos.x + plotSize.x, plotPos.y + plotSize.y};
  const ImPlotRect plotLims = ImPlot::GetPlotLimits();

  if (ImPlot::BeginItem(label)) {
    auto ctx = WaveContext::Get()->pool.GetOrAddByKey(ImGui::GetID(label));
    if (plotSize.x > 0 && plotSize.y > 0) {
      ctx->image = std::make_unique<Image>(plotSize.x / 4, plotSize.y / 4);
      ctx->image->fill(Color::Transparent);
      // border
      ctx->image->fillRect(Color::Vga::BrightRed);
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

} // namespace PixelPlot
} // namespace Mi
