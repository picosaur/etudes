#include "MapWidget.h"
#include "ImMapPlot.h"
#include "EmFetcher.h"

namespace Gui
{
    class MapWidget::Impl
    {
    public:
        std::string tileUrl{"https://a.tile.openstreetmap.org/{z}/{x}/{y}.png"};
        //std::vector<std::unique_ptr<Em::Fetcher>> fetchers;
        //std::unordered_map<std::array<int,3>, std::unique_ptr<Image>> images;
    };

    MapWidget::MapWidget() : impl_{std::make_unique<Impl>()} {}

    MapWidget::~MapWidget() {}

    void MapWidget::show()
    {
        if (ImMapPlot::BeginMapPlot("MapPlot", {-1, -1}))
        {
            ImMapPlot::SetupMapPlot();
            /*
            ImMapPlot::PlotMap("MapMap", [this](int x, int y, int z)
                               { return getTileTex(x, y, z); });*/
            ImMapPlot::PlotTileGrid("MapGrid");
            ImMapPlot::EndMapPlot();
        }
    }

    ImTextureID MapWidget::getTileTex(int x, int y, int z)
    {
        auto url = getTileUrl(x,y,z);
        // std::make_unique<Em::Fetcher>();
        return  {};
    }

    std::string MapWidget::getTileUrl(int x, int y, int z)
    {
        std::string url{impl_->tileUrl};
        const auto pos_z{url.find("{z}")};
        if (pos_z < url.size())
        {
            url.replace(pos_z, 3, std::to_string(z));
        }
        const auto pos_x{url.find("{x}")};
        if (pos_x < url.size())
        {
            url.replace(pos_x, 3, std::to_string(x));
        }
        const auto pos_y{url.find("{y}")};
        if (pos_y < url.size())
        {
            url.replace(pos_y, 3, std::to_string(y));
        }
        return url;
    }
}