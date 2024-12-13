#pragma once
#include <cmath>
#include <functional>
#include <imgui.h>
#include <implot.h>
#include <string>

namespace EtGui {
namespace MapPlot {

// Consts
// --------------------------------------------------------------------
static constexpr ImPlotFlags DefaultPlotFlags{ImPlotFlags_Equal |
                                              ImPlotFlags_NoLegend};

static constexpr ImPlotAxisFlags DefaultAxisXFlags{
    ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoGridLines |
    ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels |
    ImPlotAxisFlags_NoInitialFit | ImPlotAxisFlags_NoMenus |
    ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight};

static constexpr ImPlotAxisFlags DefaultAxisYFlags{DefaultAxisXFlags |
                                                   ImPlotAxisFlags_Invert};

static constexpr float DefaultTileSize{256.f};

// TileIndex
// --------------------------------------------------------------------
struct TileIndex {
  int x, y, z;
  constexpr TileIndex() : x(0), y(0), z(0) {}
  constexpr TileIndex(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
  int &operator[](size_t idx) {
    IM_ASSERT(idx == 0 || idx == 1);
    return ((int *)(void *)(char *)this)[idx];
  }
  int operator[](size_t idx) const {
    IM_ASSERT(idx == 0 || idx == 1);
    return ((const int *)(const void *)(const char *)this)[idx];
  }
  bool operator==(const TileIndex &other) const {
    return this->x == other.x && this->y == other.y && this->z == other.z;
  }
};

// MapGeom
// ------------------------------------------------------------------------
struct MapGeometry {
  int minZoom{0}, maxZoom{18};
  ImPlotRect plotLims{};
  ImVec2 plotSize{};
  int zoom{};
  int tilesNum{};
  double tileSizeScaled{};
  int minTX{}, maxTX{}, minTY{}, maxTY{};

  MapGeometry() {}

  MapGeometry(const ImVec2 &plotSize_, const ImPlotRect &plotLims_,
              double tileSize = DefaultTileSize) {
    plotSize = plotSize_;
    plotLims = plotLims_;

    double screenTilesF = double(plotSize.x) / tileSize;
    double totalTilesF = 1. / plotLims_.X.Size() * screenTilesF;
    double zoomF = ceil(log(totalTilesF) / log(2.0));

    zoom = std::clamp(int(zoomF) - 1, minZoom, maxZoom);
    tilesNum = pow(2, double(zoom));
    tileSizeScaled = 1. / double(tilesNum);

    minTX = std::clamp(int(plotLims.X.Min * tilesNum), 0, tilesNum - 1);
    maxTX = std::clamp(int(plotLims.X.Max * tilesNum), 0, tilesNum - 1);
    minTY = std::clamp(int(plotLims.Y.Min * tilesNum), 0, tilesNum - 1);
    maxTY = std::clamp(int(plotLims.Y.Max * tilesNum), 0, tilesNum - 1);
  }
};

// Plotters
// --------------------------------------------------------------------
using TileGetter = std::function<ImTextureID(const TileIndex &)>;

using TileCleaner = std::function<ImTextureID(const std::vector<TileIndex> &)>;

bool BeginMapPlot(const char *title_id, const ImVec2 &size = {-1.f, -1.f},
                  ImPlotFlags flags = DefaultPlotFlags);

void SetupMapPlot(ImPlotAxisFlags xFlags = ImPlotAxisFlags_None,
                  ImPlotAxisFlags yFlags = ImPlotAxisFlags_Invert);

void EndMapPlot();

MapGeometry GetMapGeometry(double tileSize = DefaultTileSize);

void PlotMap(const char *label_id, const TileGetter &getter,
             const TileCleaner &cleaner = {}, float tileSize = DefaultTileSize,
             const ImVec2 &uv0 = {0, 1}, const ImVec2 &uv1 = {1, 0});

void PlotTileGrid(const char *label_id, float tileSize = DefaultTileSize,
                  ImU32 col = IM_COL32_WHITE, float thickness = 1.0f);

void PlotRect(const char *label_id, ImPlotPoint bmin, ImPlotPoint bmax,
              ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0,
              float thickness = 1.0f);

// Helpers
// --------------------------------------------------------------------
std::string GetTileLabel(const TileIndex &index);

std::string GetTileUrl(const TileIndex &index,
                       const std::string &baseUrl =
                           "https://a.tile.openstreetmap.org/{z}/{x}/{y}.png");

ImPlotPoint GetCenterPoint(const ImPlotPoint &p1, const ImPlotPoint &p2);

} // namespace MapPlot
} // namespace EtGui
