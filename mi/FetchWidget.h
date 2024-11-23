#pragma once
#include <memory>

namespace Mi
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