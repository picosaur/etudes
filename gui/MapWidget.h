#pragma once
#include <memory>
#include <imgui.h>

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
        ImTextureID getTileTex(int x, int y, int z);
        std::string getTileUrl(int x, int y, int z);
    };
}