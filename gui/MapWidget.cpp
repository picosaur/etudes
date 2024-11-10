#include "MapWidget.h"
#include "ImMapPlot.h"
#include "EmFetcher.h"
#include "ImImage.h"

namespace Gui
{
    template <typename T>
    class XyzListItem
    {
    public:
        int x{}, y{}, z{};
        std::unique_ptr<T> value;
    };

    template <typename T>
    class XyzList
    {
        std::vector<XyzListItem<T>> list_;

    public:
        auto begin() { return list_.begin(); }

        auto end() { return list_.end(); }

        auto find(int x, int y, int z)
        {
            auto cmp = [x, y, z](const auto &item)
            {
                return item.x == x && item.y == y && item.z == z;
            };
            return std::find_if(list_.begin(), list_.end(), cmp);
        }

        auto findEmpty()
        {
            auto cmp = [](const auto &item)
            {
                return !item.value;
            };
            return std::find_if(list_.begin(), list_.end(), cmp);
        }

        auto insert(int x, int y, int z, T value)
        {
            auto it{list_.end()};
            if (it = find(x, y, z); it != end())
            {
                it->value = std::make_unique<T>(std::move(value));
            }
            else if (it = findEmpty(); it != end())
            {
                (*it) = {.x = x, .y = y, .z = z, .value = std::make_unique<T>(std::move(value))};
            }
            else
            {
                list_.push_back({.x = x, .y = y, .z = z, .value = std::make_unique<T>(std::move(value))});
                it = std::prev(list_.end());
            }
            return it;
        }
    };

    using Fetcher = Em::Fetcher;
    using Image = ImImage;

    class TileListItem
    {
    public:
        std::unique_ptr<Fetcher> fetcher;
        std::unique_ptr<Image> image;
    };
    using TileList = XyzList<TileListItem>;

    class MapWidget::Impl
    {
    public:
        std::string tileUrl{"https://a.tile.openstreetmap.org/{z}/{x}/{y}.png"};
        TileList tiles;
    };

    MapWidget::MapWidget() : impl_{std::make_unique<Impl>()} {}

    MapWidget::~MapWidget() {}

    void MapWidget::show()
    {
        if (ImMapPlot::BeginMapPlot("MapPlot", {-1, -1}))
        {
            ImMapPlot::SetupMapPlot();
            ImMapPlot::PlotMap("MapMap", [this](int x, int y, int z)
                               { return getTileTex(x, y, z); });
            ImMapPlot::PlotTileGrid("MapGrid");
            ImMapPlot::EndMapPlot();
        }
    }

    ImTextureID MapWidget::getTileTex(int x, int y, int z)
    {
        if (auto it{impl_->tiles.find(x, y, z)}; it != impl_->tiles.end())
        {
            if (it->value->image) {
                return it->value->image->textureId();
            }
            if (it->value->fetcher && it->value->fetcher->isDone()) {
                it->value->image = std::make_unique<Image>(it->value->fetcher->data(), it->value->fetcher->dataSize());
                it->value->fetcher = {};
                return it->value->image->textureId();
            }
        } else {
            impl_->tiles.insert(x,y,z, {.fetcher = std::make_unique<Fetcher>(getTileUrl(x,y,z))});
        }

        return 0;
    }

    std::string MapWidget::getTileUrl(int x, int y, int z)
    {
        std::string url{impl_->tileUrl};
        if (auto pos_x{url.find("{x}")}; pos_x < url.size())
        {
            url.replace(pos_x, 3, std::to_string(x));
        }
        if (auto pos_y{url.find("{y}")}; pos_y < url.size())
        {
            url.replace(pos_y, 3, std::to_string(y));
        }
        if (auto pos_z{url.find("{z}")}; pos_z < url.size())
        {
            url.replace(pos_z, 3, std::to_string(z));
        }
        return url;
    }
}