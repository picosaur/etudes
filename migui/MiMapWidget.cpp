#include "MiMapWidget.h"
#include "EmHttp.h"
#include "EmHttpFetcher.h"
#include "MiImage.h"
#include "MiMapPlot.h"
#include <imgui_stdlib.h>

namespace Mi {
// KeyList
// ----------------------------------------------------------------------------
template <typename Tk, typename Tv> class KeyListItem {
public:
  Tk key;
  std::unique_ptr<Tv> value;
};

template <typename Tk, typename Tv> class KeyList {
  std::vector<KeyListItem<Tk, Tv>> list_;

public:
  auto size() const { return list_.size(); }

  auto begin() { return list_.begin(); }

  auto end() { return list_.end(); }

  auto find(const Tk &key) {
    auto cmp = [key](const auto &item) { return item.key == key; };
    return std::find_if(list_.begin(), list_.end(), cmp);
  }

  auto findEmpty() {
    auto cmp = [](const auto &item) { return !item.value; };
    return std::find_if(list_.begin(), list_.end(), cmp);
  }

  auto insert(const Tk &key, Tv value) {
    auto it{list_.end()};
    if (it = find(key); it != end()) {
      it->value = std::make_unique<Tv>(std::move(value));
    } else if (it = findEmpty(); it != end()) {
      (*it) = {.key = key, .value = std::make_unique<Tv>(std::move(value))};
    } else {
      list_.push_back(
          {.key = key, .value = std::make_unique<Tv>(std::move(value))});
      it = std::prev(list_.end());
    }
    return it;
  }

  void for_each(const std::function<void(KeyListItem<Tk, Tv> &)> &func) {
    std::for_each(list_.begin(), list_.end(), func);
  }

  void erase_if(const std::function<bool(KeyListItem<Tk, Tv> &)> &func) {
    list_.erase(std::remove_if(list_.begin(), list_.end(), func), list_.end());
  }
};

// TileList
// ----------------------------------------------------------------------------
class TileListItem {
public:
  std::unique_ptr<Em::HttpFetcher> fetcher;
  std::unique_ptr<Image> image;
  bool used{};
};

class TileList {
  KeyList<MapPlot::TileIndex, TileListItem> list_;

public:
  static auto &tileTrash() {
    static std::vector<std::unique_ptr<TileListItem>> trash;
    return trash;
  };

  TileList() {}
  TileList(const TileList &) = delete;
  TileList operator=(const TileList &) = delete;
  ~TileList() { tileTrash().clear(); }

  void clear() {
    for (auto &item : list_) {
      tileTrash().push_back(std::move(item.value));
    }
    list_.erase_if([](auto &item) { return !item.value; });
  }

  void setUnused() {
    list_.for_each([](auto &item) { item.value->used = {}; });
  }

  void removeUnused() {
    list_.for_each([this](auto &item) {
      if (!item.value->used) {
        tileTrash().push_back(std::move(item.value));
      }
    });
    list_.erase_if([](auto &item) { return !item.value; });
    tileTrash().erase(std::remove_if(tileTrash().begin(), tileTrash().end(),
                                     [](const auto &item) {
                                       return item->fetcher &&
                                              item->fetcher->isDone();
                                     }),
                      tileTrash().end());
  }

  ImTextureID getTile(const MapPlot::TileIndex &index, const std::string &url) {
    if (auto it{list_.find(index)}; it != list_.end()) {
      it->value->used = true;
      if (it->value->image) {
        return it->value->image->textureId();
      }
      if (it->value->fetcher && it->value->fetcher->isDone()) {
        it->value->image = std::make_unique<Image>(
            it->value->fetcher->data(), it->value->fetcher->dataSize());
        it->value->fetcher = {};
        return it->value->image->textureId();
      }
    } else {
      list_.insert(
          index, {.fetcher = std::make_unique<Em::HttpFetcher>(
                      MapPlot::GetTileUrl(index, url),
                      Em::Http::BrowserRequestHeaders(Em::Http::UrlHost(url))),
                  .used = true});
    }
    return 0;
  }
};

// TileMap
// ----------------------------------------------------------------------------
class TileMap {
public:
  static constexpr const char *A_TILE_OPENSTREETMAP{
      "https://a.tile.openstreetmap.org/{z}/{x}/{y}.png"};
  static constexpr const char *T2_OPENSEAMAP{
      "https://t2.openseamap.org/tile/{z}/{x}/{y}.png"};
  static constexpr const char *TILES_OPENSEAMAP{
      "https://tiles.openseamap.org/seamark/{z}/{x}/{y}.png"};
  static constexpr const char *ARC_IMAGERY{
      "https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/"
      "MapServer/tile/{z}/{y}/{x}"};

  bool enabled{};
  std::string url{A_TILE_OPENSTREETMAP};
  TileList tiles;

  static float getUrlInpWidth() {
    static float w{};
    if (w < 1.f) {
      w = ImGui::CalcTextSize(
              "https://reference-text-used-for-calc-width/{z}/{x}/{y}.png")
              .x;
      w *= 2.0;
    }
    return w;
  }

  TileMap() {}

  TileMap(const std::string &url_, bool enabled_ = true)
      : url{url_}, enabled{enabled_} {}

  void showInputs() {
    ImGui::PushID(this);
    ImGui::Checkbox("Enabled", &enabled);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(getUrlInpWidth());
    ImGui::InputText("URL", &url);
    ImGui::SameLine();
    if (ImGui::Button("Apply")) {
      tiles.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("OSM-A")) {
      url = A_TILE_OPENSTREETMAP;
      tiles.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("SEA-T2")) {
      url = T2_OPENSEAMAP;
      tiles.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("SEA-TILES")) {
      url = TILES_OPENSEAMAP;
      tiles.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("ARC-IMAGERY")) {
      url = ARC_IMAGERY;
      tiles.clear();
    }

    // ImGui::Text("Tiles %zu", tiles.size());
    ImGui::PopID();
  }

  void plotMap() {
    if (enabled) {
      ImGui::PushID(this);
      tiles.setUnused();
      MapPlot::PlotMap("##", [this](const auto &index) {
        return tiles.getTile(index, url);
      });
      tiles.removeUnused();
      ImGui::PopID();
    }
  }
};

// MapWidget
// ----------------------------------------------------------------------------
class MapWidget::Impl {
public:
  TileMap tileMap1{TileMap::ARC_IMAGERY};
  TileMap tileMap2{TileMap::TILES_OPENSEAMAP};
  MapPlot::MapGeometry geom;

  // test
  double xmin{}, xmax{};
};

MapWidget::MapWidget() : impl_{std::make_unique<Impl>()} {}

MapWidget::~MapWidget() {}

void MapWidget::show() {
  // Layer 1
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Layer 1");
  ImGui::SameLine();
  impl_->tileMap1.showInputs();

  // Layer 2
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted("Layer 2");
  ImGui::SameLine();
  impl_->tileMap2.showInputs();

  // Buttons
  if (ImGui::Button("A")) {
    impl_->xmin = 0.5;
    impl_->xmax = 0.6;
  }

  if (MapPlot::BeginMapPlot("##", {-1, -1})) {
    if (impl_->xmin > 0 && impl_->xmax > 0) {
      ImPlot::SetupAxisLimits(ImAxis_X1, impl_->xmin, impl_->xmax,
                              ImPlotCond_Always);
      impl_->xmin = {};
      impl_->xmax = {};
    }
    MapPlot::SetupMapPlot();
    impl_->geom = MapPlot::GetMapGeometry();
    impl_->tileMap1.plotMap();
    impl_->tileMap2.plotMap();
    MapPlot::PlotTileGrid("MapGrid");
    MapPlot::EndMapPlot();
  }
}
} // namespace Mi
