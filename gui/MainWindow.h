#pragma once
#include <memory>

namespace gui
{
    class MainWindow
    {
        class Impl;
        std::unique_ptr<Impl> impl_;

    public:
        MainWindow();
        ~MainWindow();

    private:
        void onShowMenus();
        void onShowToolbar();
    };
}