#pragma once
#include <imgui.h>
#include <implot.h>

namespace Mi {
class Image;

namespace WavePlot {

void PlotPixelWave(const char *label, double *ys, int count);

void DrawWaveOnImage(Image *image, double *ys, int count,
                     const ImPlotRect &plotLims, ImU32 color);

void PlotShadedWave(const char *label, double *ys, int count);

} // namespace WavePlot
} // namespace Mi
