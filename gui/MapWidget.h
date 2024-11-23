#pragma once
#include <memory>
#include <imgui.h>
#include "ImMapPlot.h"

namespace Gui
{
    class MapWidget
    {
        class Impl;
        std::unique_ptr<Impl> impl_;

    public:
        MapWidget();
        ~MapWidget();

        void show();

    private:
        ImTextureID getTileTex(const ImMapPlot::TileIndex& index);
    };
}