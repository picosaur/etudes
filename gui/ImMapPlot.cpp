#include "ImMapPlot.h"
#include <implot.h>
#include <algorithm>
#include <implot_internal.h>
#include <imgui_internal.h>
#include <vector>
#include <string>

namespace ImMapPlot
{
    static constexpr double PI{3.14159265358979323846};
    static constexpr double PI2{PI * 2.0};
    static constexpr double RAD{PI / 180.0};
    static constexpr double DEG{180.0 / PI};
    static constexpr double R_EARTH{6371e3};
    static constexpr double MIN_LAT{-85.0};
    static constexpr double MAX_LAT{+85.0};
    static constexpr double MIN_LON{-179.9};
    static constexpr double MAX_LON{+179.9};
    static constexpr int MIN_ZOOM{0};
    static constexpr int MAX_ZOOM{18};

    static constexpr int POW2[]{
        (1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6),
        (1 << 7), (1 << 8), (1 << 9), (1 << 10), (1 << 11), (1 << 12), (1 << 13),
        (1 << 14), (1 << 15), (1 << 16), (1 << 17), (1 << 18)};

    double dmsToDeg(double d, double m, double s, char nsew)
    {
        double deg{d + m / 60.0 + s / 3600.0};
        if (nsew == 'S' || nsew == 'W')
        {
            deg = -deg;
        }
        return deg;
    }

    double distance(double lat1, double lon1, double lat2, double lon2)
    {
        lat1 *= RAD;
        lon1 *= RAD;
        lat2 *= RAD;
        lon2 *= RAD;

        const double dlat{(lat2 - lat1) / 2.0};
        const double dlon{(lon2 - lon1) / 2.0};
        const double a{sin(dlat) * sin(dlat) +
                       cos(lat1) * cos(lat2) * sin(dlon) * sin(dlon)};
        const double c{2.0 * atan2(sqrt(a), sqrt(1.0 - a))};
        const double d{R_EARTH * c};

        return d;
    }

    double bearing(double lat1, double lon1, double lat2, double lon2)
    {
        lat1 *= RAD;
        lon1 *= RAD;
        lat2 *= RAD;
        lon2 *= RAD;

        const double dlon{lon2 - lon1};
        const double x{cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon)};
        const double y{sin(dlon) * cos(lat2)};
        const double th{atan2(y, x)};
        const double b{fmod(th * DEG + 360.0, 360.0)};

        return b;
    }

    void midpoint(double &lat, double &lon, double lat1, double lon1,
                  double lat2, double lon2)
    {
        lat1 *= RAD;
        lon1 *= RAD;
        lat2 *= RAD;
        lon2 *= RAD;

        const double dlon{lon2 - lon1};
        const double Bx = cos(lat2) * cos(dlon);
        const double By = cos(lat2) * sin(dlon);
        const double y = sin(lat1) + sin(lat2);
        const double x = sqrt((cos(lat1) + Bx) * (cos(lat1) + Bx) + By * By);
        lat = atan2(y, x);
        lon = lon1 + atan2(By, cos(lat1) + Bx);

        lat *= DEG;
        lon *= DEG;
        lon = fmod(lon + 540.0, 360.0) - 180.0;
    }

    void destination(double &lat, double &lon, double lat1, double lon1,
                     double d, double b)
    {

        lat1 *= RAD;
        lon1 *= RAD;
        b *= RAD;

        const double delta{d / R_EARTH};
        const double sin_lat2{sin(lat1) * cos(delta) +
                              cos(lat1) * sin(delta) * cos(b)};
        const double y{sin(b) * sin(delta) * cos(lat1)};
        const double x{cos(delta) - sin(lat1) * sin_lat2};
        lat = asin(sin_lat2);
        lon = lon1 + atan2(y, x);

        lat *= DEG;
        lon *= DEG;
        lon = fmod((lon + 540.0), 360.0) - 180.0;
    }

    void cartesian(double &x, double &y, double lat, double lon, double lat0, double lon0)
    {
        lat0 *= RAD;
        lon0 *= RAD;
        lat *= RAD;
        lon *= RAD;

        const double dlon{lon - lon0};
        x = dlon * cos(lat / 2.0 + lat0 / 2.0) * R_EARTH;
        y = (lat - lat0) * R_EARTH;
    }

    double lon2x(double lon, int z)
    {
        return (lon + 180.0) / 360.0 * double(POW2[z]);
    }

    double lat2y(double lat, int z)
    {
        return (1.0 - asinh(tan(lat * RAD)) / PI) / 2.0 * double(POW2[z]);
    }

    double x2lon(double x, int z)
    {
        return x / double(POW2[z]) * 360.0 - 180.0;
    }

    double y2lat(double y, int z)
    {
        const double n{PI - PI2 * y / double(POW2[z])};
        return DEG * atan(0.5 * (exp(n) - exp(-n)));
    }

    int lon2tx(double lon, int z)
    {
        return int(floor(lon2x(lon, z)));
    }

    int lat2ty(double lat, int z)
    {
        return int(floor(lat2y(lat, z)));
    }

    // MapPlot
    //-------------------------------------------------------------------------

    bool BeginMapPlot(const char *title_id, const ImVec2 &size, ImPlotFlags flags)
    {
        return ImPlot::BeginPlot(title_id, size, flags);
    }

    void SetupMapPlot()
    {
        // ImPlot::SetupAxis(ImAxis_X1, nullptr, impl_->xFlags);
        // ImPlot::SetupAxis(ImAxis_Y1, nullptr, impl_->yFlags);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -1.0, 2.0);

        ImPlot::GetInputMap().ZoomRate = 0.1;

        return ImPlot::SetupFinish();
    }

    void EndMapPlot()
    {
        return ImPlot::EndPlot();
    }

    // Map
    // ------------------------------------------------------------------------
    class MapData
    {
    public:
        std::vector<std::unique_ptr<std::string>> tileGridStrings;

        const char *addString(const std::string &s)
        {
            tileGridStrings.push_back(std::make_unique<std::string>(s));
            return tileGridStrings.back()->c_str();
        }
    };

    class MapContext
    {
        MapContext() {}

    public:
        MapContext(MapContext const &) = delete;
        void operator=(MapContext const &) = delete;

        static MapContext *Get()
        {
            static MapContext instance;
            return &instance;
        }

        ImPool<MapData> maps;
    };

    class MapGeom
    {
    public:
        MapGeom(double tileSize)
        {
            plotLims = ImPlot::GetPlotLimits(ImAxis_X1, ImAxis_Y1);
            plotSize = ImPlot::GetPlotSize(); // pixels

            tileRes = std::max(plotSize.x / plotLims.X.Size(), plotSize.y / plotLims.Y.Size());
            zoom = std::clamp(int(floor(log2(tileRes / tileSize))), MIN_ZOOM, MAX_ZOOM);
            tilesNum = POW2[zoom];
            tileSizeScaled = 1. / double(tilesNum);

            minTX = std::clamp(int(plotLims.X.Min * tilesNum), 0, tilesNum - 1);
            maxTX = std::clamp(int(plotLims.X.Max * tilesNum), 0, tilesNum - 1);
            minTY = std::clamp(int(plotLims.Y.Min * tilesNum), 0, tilesNum - 1);
            maxTY = std::clamp(int(plotLims.Y.Max * tilesNum), 0, tilesNum - 1);
        }

        ImPlotRect plotLims{};
        ImVec2 plotSize{};
        double tileRes{};
        int zoom{};
        int tilesNum{};
        double tileSizeScaled{};
        int minTX{}, maxTX{}, minTY{}, maxTY{};
    };

    std::string GetTileLabel(int tx, int ty, int tz)
    {
        return std::to_string(tx) + '/' + std::to_string(ty) + '/' + std::to_string(tz);
    }

    ImPlotPoint GetCenterPoint(const ImPlotPoint &p1, const ImPlotPoint &p2)
    {
        return {(p1.x + p2.x) / 2., (p1.y + p2.y) / 2.};
    }

    void PlotMap(const char *label_id, const TileGetter &getter, float tileSize, const ImVec2 &uv0, const ImVec2 &uv1)
    {
        auto *mapCtx = MapContext::Get()->maps.GetOrAddByKey(ImGui::GetID(label_id));
        auto mapGeom = MapGeom(tileSize);

        ImPlotPoint bmin{}, bmax{};
        for (auto x{mapGeom.minTX}; x < mapGeom.maxTX + 1; ++x)
        {
            bmin.x = double(x) * mapGeom.tileSizeScaled;
            bmax.x = double(x + 1) * mapGeom.tileSizeScaled;
            for (auto y{mapGeom.minTY}; y != mapGeom.maxTY + 1; ++y)
            {
                bmin.y = double(y) * mapGeom.tileSizeScaled;
                bmax.y = double(y + 1) * mapGeom.tileSizeScaled;
                ImPlot::PlotImage("##", getter(x, y, mapGeom.zoom), bmin, bmax);
            }
        }
    }

    void PlotTileGrid(const char *label_id, float tileSize, ImU32 color, float thickness)
    {
        auto *mapCtx = MapContext::Get()->maps.GetOrAddByKey(ImGui::GetID(label_id));
        mapCtx->tileGridStrings.clear();
        auto mapGeom = MapGeom(tileSize);

        ImPlotPoint bmin{}, bmax{};
        for (auto x{mapGeom.minTX}; x < mapGeom.maxTX + 1; ++x)
        {
            bmin.x = double(x) * mapGeom.tileSizeScaled;
            bmax.x = double(x + 1) * mapGeom.tileSizeScaled;
            for (auto y{mapGeom.minTY}; y != mapGeom.maxTY + 1; ++y)
            {
                bmin.y = double(y) * mapGeom.tileSizeScaled;
                bmax.y = double(y + 1) * mapGeom.tileSizeScaled;
                auto center = GetCenterPoint(bmin, bmax);
                PlotRect("##", bmin, bmax, color);
                ImPlot::PlotText(mapCtx->addString(GetTileLabel(x, y, mapGeom.zoom)), center.x, center.y);
            }
        }
    }

    void PlotRect(const char *label_id, ImPlotPoint bmin, ImPlotPoint bmax, ImU32 col, float rounding, ImDrawFlags flags, float thickness)
    {
        if (ImPlot::BeginItem(label_id))
        {
            ImDrawList &draw_list = *ImPlot::GetPlotDrawList();
            ImVec2 p1 = ImPlot::PlotToPixels(bmin.x, bmax.y, IMPLOT_AUTO, IMPLOT_AUTO);
            ImVec2 p2 = ImPlot::PlotToPixels(bmax.x, bmin.y, IMPLOT_AUTO, IMPLOT_AUTO);
            ImPlot::PushPlotClipRect();
            draw_list.AddRect(p1, p2, col);
            ImPlot::PopPlotClipRect();
            ImPlot::EndItem();
        }
    }
}