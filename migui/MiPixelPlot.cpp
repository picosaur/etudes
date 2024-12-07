#include "MiPixelPlot.h"
#include "MiColor.h"
#include "MiContext.h"
#include "MiImage.h"
#include <algorithm>
#include <implot.h>
#include <implot_internal.h>
#include <limits>

namespace Mi {
namespace PixelPlot {

class WaveData {
public:
  std::unique_ptr<Image> image;
};

using WaveContext = Context<WaveData>;

void PlotPixelWave(const char *label, double *ys, int count) {
  auto ctx = WaveContext::Get()->pool.GetOrAddByKey(ImGui::GetID(label));

  const ImVec2 plotPos = ImPlot::GetPlotPos();
  const ImVec2 plotSize = ImPlot::GetPlotSize();
  const ImVec2 plotPosEnd = {plotPos.x + plotSize.x, plotPos.y + plotSize.y};
  const ImPlotRect plotLims = ImPlot::GetPlotLimits();

  if (ImPlot::BeginItem(label)) {
    if (plotSize.x > 0 && plotSize.y > 0) {
      ctx->image = std::make_unique<Image>(plotSize.x / 4, plotSize.y / 4);
      ctx->image->fill(Color::Transparent);
      // test
      ctx->image->fillRow(10, Color::Vga::BrightBlue);
      ctx->image->fillCol(2, Color::Vga::BrightRed, 0, 2);
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
}

void DrawWaveOnImage(Image *image, double *ys, int count,
                     const ImPlotRect &plotLims, ImU32 color) {
  const double loVal = {std::numeric_limits<double>::lowest()};
  const double hiVal = {std::numeric_limits<double>::max()};

  const double dx = plotLims.X.Size() / double(image->width());
  const double dy = plotLims.Y.Size() / double(image->height());

  double hi{loVal};
  double lo{hiVal};

  double x{}; // starts from 0;
  double xx{};
  int j{int((0 - plotLims.X.Min) / dx)};

  for (int i{}; i < count; ++i) {
    if (xx >= dx) {
      if (j >= 0 && j < image->width()) {
        int k0 = (lo - plotLims.Y.Min) / dy - 4;
        int k1 = (hi - plotLims.Y.Min) / dy + 4;
        k0 = std::clamp(k0, 0, image->height() - 1);
        k1 = std::clamp(k1, 0, image->height() - 1);
        image->fillCol(j, color, k0, k1);
      }
      lo = hiVal;
      hi = loVal;
      xx = {};
      j++;
    }
    lo = std::min(lo, ys[i]);
    hi = std::max(hi, ys[i]);
    x += 1;
    xx += 1;
  }
}

} // namespace PixelPlot
} // namespace Mi
