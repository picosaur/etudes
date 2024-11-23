#include "MapWidget.h"
#include "ImMapPlot.h"
#include "EmFetcher.h"
#include "ImImage.h"

namespace Gui
{
    template <typename Tk, typename Tv>
    class KeyListItem
    {
    public:
        Tk key;
        std::unique_ptr<Tv> value;
    };

    template <typename Tk, typename Tv>
    class KeyList
    {
        std::vector<KeyListItem<Tk,Tv>> list_;

    public:
        auto begin() { return list_.begin(); }

        auto end() { return list_.end(); }

        auto find(const Tk& key)
        {
            auto cmp = [key](const auto &item)
            {
                return item.key == key;
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

        auto insert(const Tk& key, Tv value)
        {
            auto it{list_.end()};
            if (it = find(key); it != end())
            {
                it->value = std::make_unique<Tv>(std::move(value));
            }
            else if (it = findEmpty(); it != end())
            {
                (*it) = {.key = key, .value = std::make_unique<Tv>(std::move(value))};
            }
            else
            {
                list_.push_back({.key = key, .value = std::make_unique<Tv>(std::move(value))});
                it = std::prev(list_.end());
            }
            return it;
        }
    };

    using Fetcher = Em::Fetcher;

    class TileListItem
    {
    public:
        std::unique_ptr<Fetcher> fetcher;
        std::unique_ptr<ImImage> image;
    };
    using TileList = KeyList<ImMapPlot::TileIndex, TileListItem>;

    class MapWidget::Impl
    {
    public:
        std::string tileUrl{"https://a.tile.openstreetmap.org/{z}/{x}/{y}.png"};
        //std::string tileUrl{"https://t2.openseamap.org/tile/{z}/{x}/{y}.png"};
        TileList tiles;
    };

    MapWidget::MapWidget() : impl_{std::make_unique<Impl>()} {}

    MapWidget::~MapWidget() {}

    void MapWidget::show()
    {
        if (ImMapPlot::BeginMapPlot("MapPlot", {-1, -1}))
        {
            ImMapPlot::SetupMapPlot();
            ImMapPlot::PlotMap("MapMap", [this](const auto& index)
                               { return getTileTex(index); });
            ImMapPlot::PlotTileGrid("MapGrid");
            ImMapPlot::EndMapPlot();
        }
    }

    ImTextureID MapWidget::getTileTex(const ImMapPlot::TileIndex& index)
    {
        if (auto it{impl_->tiles.find(index)}; it != impl_->tiles.end())
        {
            if (it->value->image) {
                return it->value->image->textureId();
            }
            if (it->value->fetcher && it->value->fetcher->isDone()) {
                it->value->image = std::make_unique<ImImage>(it->value->fetcher->data(), it->value->fetcher->dataSize());
                it->value->fetcher = {};
                return it->value->image->textureId();
            }
        } else {
            impl_->tiles.insert(index, {.fetcher = std::make_unique<Fetcher>(ImMapPlot::GetTileUrl(index, impl_->tileUrl))});
        }
        return 0;
    }
}