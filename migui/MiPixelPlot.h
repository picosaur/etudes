#pragma once
#include <imgui.h>
#include <implot.h>

namespace Mi {
class Image;

namespace PixelPlot {

void PlotPixelWave(const char *label, double *ys, int count);

void DrawWaveOnImage(Image *image, double *ys, int count,
                     const ImPlotRect &plotLims);

} // namespace PixelPlot
} // namespace Mi
