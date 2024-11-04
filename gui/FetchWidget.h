#pragma once
#include <memory>
#include "EmFetcher.h"

namespace gui
{
    class FetchWidget
    {
        class Impl;
        std::unique_ptr<Impl> impl_;

    public:
        FetchWidget();
        ~FetchWidget();

        void show();
    };
}