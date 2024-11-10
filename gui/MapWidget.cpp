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

        auto insert(int x, int y, int z, std::unique_ptr<T> ptr)
        {
            auto it{list_.end()};
            if (it = find(x, y, z); it != end())
            {
                it->value = std::move(ptr);
            }
            else if (it = findEmpty(); it != end())
            {
                (*it) = {.x = x, .y = y, .z = z, .value = std::move(ptr)};
            }
            else
            {
                list_.push_back({.x = x, .y = y, .z = z, .value = std::move(ptr)});
                it = std::prev(list_.end());
            }
            return it;
        }
    };

    using TileList = XyzList<ImImage>;

    using Fetcher = Em::Fetcher;
    using FetcherList = XyzList<Fetcher>;

    class MapWidget::Impl
    {
    public:
        std::string tileUrl{"https://a.tile.openstreetmap.org/{z}/{x}/{y}.png"};
        TileList tiles;
        FetcherList fetchers;
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
        if (auto it{impl_->tiles.find(x,y,z)}; it != impl_->tiles.end())
        {
            return it->value->textureId();
        }

        if (auto it{impl_->fetchers.find(x,y,z)}; it != impl_->fetchers.end())
        {
            if (it->value->isDone())
            {
                auto jt = impl_->tiles.insert(x,y,z, std::make_unique<ImImage>(it->value->data(), it->value->dataSize()));
                return jt->value->textureId();
            }
        }
        else
        {
            impl_->fetchers.insert(x,y,z, std::make_unique<Fetcher>(getTileUrl(x,y,z)));
        }
        return 0;
    }

    std::string MapWidget::getTileUrl(int x, int y, int z)
    {
        std::string url{impl_->tileUrl};
        if (auto pos_z{url.find("{z}")}; pos_z < url.size())
        {
            url.replace(pos_z, 3, std::to_string(z));
        }
        if (auto pos_x{url.find("{x}")}; pos_x < url.size())
        {
            url.replace(pos_x, 3, std::to_string(x));
        }
        if (auto pos_y{url.find("{y}")}; pos_y < url.size())
        {
            url.replace(pos_y, 3, std::to_string(y));
        }
        return url;
    }
}