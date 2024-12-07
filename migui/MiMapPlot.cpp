#include "MiMapPlot.h"
#include "MiContext.h"
#include <imgui_internal.h>
#include <implot.h>
#include <implot_internal.h>
#include <memory>
#include <string>

namespace Mi {
namespace MapPlot {

// MapData
// ------------------------------------------------------------------------
class MapData {
public:
};

using MapContext = Context<MapData>;

// GridData
// ------------------------------------------------------------------------
class GridData {
public:
  std::vector<std::unique_ptr<std::string>> strings;

  const char *addString(const std::string &s) {
    strings.push_back(std::make_unique<std::string>(s));
    return strings.back()->c_str();
  }
};

using GridContext = Context<GridData>;

// Plotters
// ------------------------------------------------------------------------
bool BeginMapPlot(const char *title_id, const ImVec2 &size, ImPlotFlags flags) {
  return ImPlot::BeginPlot(title_id, size, flags);
}

void SetupMapPlot(ImPlotAxisFlags xFlags, ImPlotAxisFlags yFlags) {
  ImPlot::SetupAxis(ImAxis_X1, nullptr, xFlags);
  ImPlot::SetupAxis(ImAxis_Y1, nullptr, yFlags);
  ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -1.0, 2.0);
  ImPlot::GetInputMap().ZoomRate = 1.0;
  return ImPlot::SetupFinish();
}

void EndMapPlot() { return ImPlot::EndPlot(); }

void PlotMap(const char *label_id, const TileGetter &getter,
             const TileCleaner &cleaner, float tileSize, const ImVec2 &uv0,
             const ImVec2 &uv1) {
  // auto *ctx = MapContext::Get()->pool.GetOrAddByKey(ImGui::GetID(label_id));
  auto geom = GetMapGeometry(tileSize);
  ImPlotPoint bmin{}, bmax{};
  for (auto x{geom.minTX}; x < geom.maxTX + 1; ++x) {
    bmin.x = double(x) * geom.tileSizeScaled;
    bmax.x = double(x + 1) * geom.tileSizeScaled;
    for (auto y{geom.minTY}; y != geom.maxTY + 1; ++y) {
      bmin.y = double(y) * geom.tileSizeScaled;
      bmax.y = double(y + 1) * geom.tileSizeScaled;
      ImPlot::PlotImage("##", getter(TileIndex(x, y, geom.zoom)), bmin, bmax,
                        uv0, uv1);
    }
  }
}

void PlotTileGrid(const char *label_id, float tileSize, ImU32 color,
                  float thickness) {
  auto *ctx = GridContext::Get()->pool.GetOrAddByKey(ImGui::GetID(label_id));
  ctx->strings.clear();
  auto geom = GetMapGeometry(tileSize);
  ImPlotPoint bmin{}, bmax{};
  for (auto x{geom.minTX}; x < geom.maxTX + 1; ++x) {
    bmin.x = double(x) * geom.tileSizeScaled;
    bmax.x = double(x + 1) * geom.tileSizeScaled;
    for (auto y{geom.minTY}; y != geom.maxTY + 1; ++y) {
      bmin.y = double(y) * geom.tileSizeScaled;
      bmax.y = double(y + 1) * geom.tileSizeScaled;
      auto center = GetCenterPoint(bmin, bmax);
      PlotRect("##", bmin, bmax, color);
      ImPlot::PlotText(ctx->addString(GetTileLabel(TileIndex(x, y, geom.zoom))),
                       center.x, center.y);
    }
  }
}

void PlotRect(const char *label_id, ImPlotPoint bmin, ImPlotPoint bmax,
              ImU32 col, float rounding, ImDrawFlags flags, float thickness) {
  if (ImPlot::BeginItem(label_id)) {
    ImDrawList &draw_list = *ImPlot::GetPlotDrawList();
    ImVec2 p1 = ImPlot::PlotToPixels(bmin.x, bmax.y, IMPLOT_AUTO, IMPLOT_AUTO);
    ImVec2 p2 = ImPlot::PlotToPixels(bmax.x, bmin.y, IMPLOT_AUTO, IMPLOT_AUTO);
    ImPlot::PushPlotClipRect();
    draw_list.AddRect(p1, p2, col);
    ImPlot::PopPlotClipRect();
    ImPlot::EndItem();
  }
}

// Helpers
//-------------------------------------------------------------------------
std::string GetTileUrl(const TileIndex &index, const std::string &baseUrl) {
  auto url{baseUrl};
  if (auto pos_x{url.find("{x}")}; pos_x < url.size()) {
    url.replace(pos_x, 3, std::to_string(index.x));
  }
  if (auto pos_y{url.find("{y}")}; pos_y < url.size()) {
    url.replace(pos_y, 3, std::to_string(index.y));
  }
  if (auto pos_z{url.find("{z}")}; pos_z < url.size()) {
    url.replace(pos_z, 3, std::to_string(index.z));
  }
  return url;
}

std::string GetTileLabel(const TileIndex &index) {
  return std::to_string(index.z) + '/' + std::to_string(index.x) + '/' +
         std::to_string(index.y);
}

ImPlotPoint GetCenterPoint(const ImPlotPoint &p1, const ImPlotPoint &p2) {
  return {(p1.x + p2.x) / 2., (p1.y + p2.y) / 2.};
}

MapGeometry GetMapGeometry(double tileSize) {
  return MapGeometry(ImPlot::GetPlotSize(),
                     ImPlot::GetPlotLimits(ImAxis_X1, ImAxis_Y1), tileSize);
}

} // namespace MapPlot
} // namespace Mi
