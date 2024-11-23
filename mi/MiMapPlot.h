#pragma once
#include <imgui.h>
#include <implot.h>
#include <memory>
#include <functional>
#include <string>

namespace Mi
{
    namespace MapPlot
    {
        // TileIndex
        // --------------------------------------------------------------------
        struct TileIndex
        {
            int x, y, z;
            constexpr TileIndex() : x(0), y(0), z(0) {}
            constexpr TileIndex(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
            int &operator[](size_t idx)
            {
                IM_ASSERT(idx == 0 || idx == 1);
                return ((int *)(void *)(char *)this)[idx];
            }
            int operator[](size_t idx) const
            {
                IM_ASSERT(idx == 0 || idx == 1);
                return ((const int *)(const void *)(const char *)this)[idx];
            }
            bool operator==(const TileIndex &other) const { return this->x == other.x && this->y == other.y && this->z == other.z; }
        };

        // Helpers
        // --------------------------------------------------------------------
        std::string GetTileLabel(const TileIndex &index);
        std::string GetTileUrl(const TileIndex &index, const std::string &baseUrl = "https://a.tile.openstreetmap.org/{z}/{x}/{y}.png");
        ImPlotPoint GetCenterPoint(const ImPlotPoint &p1, const ImPlotPoint &p2);

        // Plotters
        // --------------------------------------------------------------------
        using TileGetter = std::function<ImTextureID(const TileIndex &)>;
        using TileCleaner = std::function<ImTextureID(const std::vector<TileIndex> &)>;

        static constexpr ImPlotFlags PlotFlags{ImPlotFlags_Equal | ImPlotFlags_NoLegend};

        static constexpr ImPlotAxisFlags AxisXFlags{
            ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoGridLines |
            ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels |
            ImPlotAxisFlags_NoInitialFit | ImPlotAxisFlags_NoMenus |
            ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight};

        static constexpr ImPlotAxisFlags AxisYFlags{AxisXFlags | ImPlotAxisFlags_Invert};

        static constexpr float DefaultTileSize{256.f};

        bool BeginMapPlot(const char *title_id, const ImVec2 &size = {-1.f, -1.f}, ImPlotFlags flags = PlotFlags);
        void SetupMapPlot(ImPlotAxisFlags xFlags = ImPlotAxisFlags_None, ImPlotAxisFlags yFlags = ImPlotAxisFlags_Invert);
        void EndMapPlot();

        void PlotMap(const char *label_id, const TileGetter &getter, const TileCleaner &cleaner = {}, float tileSize = DefaultTileSize, const ImVec2 &uv0 = {0, 1}, const ImVec2 &uv1 = {1, 0});
        void PlotTileGrid(const char *label_id, float tileSize = DefaultTileSize, ImU32 col = IM_COL32_WHITE, float thickness = 1.0f);
        void PlotRect(const char *label_id, ImPlotPoint bmin, ImPlotPoint bmax, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);
    }
}