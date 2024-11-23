#include "MiMapWidget.h"
#include "MiMapPlot.h"
#include "EmFetcher.h"
#include "MiImage.h"
#include <iostream>

namespace Mi
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
        std::vector<KeyListItem<Tk, Tv>> list_;

    public:
        auto size() const { return list_.size(); }

        auto begin() { return list_.begin(); }

        auto end() { return list_.end(); }

        auto find(const Tk &key)
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

        auto insert(const Tk &key, Tv value)
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

        void for_each(const std::function<void(KeyListItem<Tk, Tv> &)> &func)
        {
            std::for_each(list_.begin(), list_.end(), func);
        }

        void erase_if(const std::function<bool(KeyListItem<Tk, Tv> &)> &func)
        {
            list_.erase(std::remove_if(list_.begin(), list_.end(), func), list_.end());
        }
    };

    
    using Fetcher = Em::Fetcher;
    using FetcherHeaders = std::unordered_map<std::string, std::string>;

    class DummyFetcher
    {
    public:
        DummyFetcher(const std::string &url, const FetcherHeaders &headers = {}) {}

        bool isDone() const { return true; }

        const std::byte *data() const { return nullptr; }
        std::size_t dataSize() const { return 0; }
    };

    class TileListItem
    {
    public:
        std::unique_ptr<Fetcher> fetcher;
        std::unique_ptr<Image> image;
        bool used{};
    };

    class TileList : public KeyList<MapPlot::TileIndex, TileListItem>
    {
    public:
        TileList() {}

        void setUnused()
        {
            for_each([](auto &item)
                     { item.value->used = {}; });
        }

        void removeUnused()
        {
            erase_if(
                [](auto &item)
                {
                    if (item.value->fetcher && !item.value->fetcher->isDone())
                    {
                        return false;
                    }
                    return !item.value->used;
                });
        }

        ImTextureID getTile(const MapPlot::TileIndex &index, const std::string &url)
        {
            if (auto it{find(index)}; it != end())
            {
                it->value->used = true;
                if (it->value->image)
                {
                    return it->value->image->textureId();
                }
                if (it->value->fetcher && it->value->fetcher->isDone())
                {
                    it->value->image = std::make_unique<Image>(it->value->fetcher->data(), it->value->fetcher->dataSize());
                    it->value->fetcher = {};
                    return it->value->image->textureId();
                }
            }
            else
            {
                insert(index, {.fetcher = std::make_unique<Fetcher>(MapPlot::GetTileUrl(index, url)), .used = true});
            }
            return 0;
        }
    };

    class MapWidget::Impl
    {
    public:
        std::string tileUrl{"https://a.tile.openstreetmap.org/{z}/{x}/{y}.png"};
        // std::string tileUrl{"https://t2.openseamap.org/tile/{z}/{x}/{y}.png"};
        TileList tiles;
    };

    MapWidget::MapWidget() : impl_{std::make_unique<Impl>()} {}

    MapWidget::~MapWidget() {}

    void MapWidget::show()
    {
        ImGui::Text("Tiles %zu", impl_->tiles.size());
        if (MapPlot::BeginMapPlot("MapPlot", {-1, -1}))
        {
            impl_->tiles.setUnused();
            MapPlot::SetupMapPlot();
            MapPlot::PlotMap("MapMap", [this](const auto &index)
                               { return impl_->tiles.getTile(index, impl_->tileUrl); });
            MapPlot::PlotTileGrid("MapGrid");
            MapPlot::EndMapPlot();
            impl_->tiles.removeUnused();
        }
    }
}