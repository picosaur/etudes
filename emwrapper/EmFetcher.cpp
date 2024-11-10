#include "EmFetcher.h"
#include <emscripten/fetch.h>
#include <stdio.h>

namespace Em
{
    Fetcher::Fetcher(const std::string &url)
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.userData = (this);
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = Fetcher::onSuccess;
        attr.onerror = Fetcher::onError;
        fetch_ = emscripten_fetch(&attr, url.c_str());
    }

    Fetcher::~Fetcher()
    {
        if (fetch_ != nullptr)
        {
            emscripten_fetch_close(fetch_);
        }
    }

    void Fetcher::onSuccess(emscripten_fetch_t *fetch)
    {
        printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
        static_cast<Fetcher *>(fetch->userData)->state_ = fetch->readyState;
    }

    void Fetcher::onError(emscripten_fetch_t *fetch)
    {
        printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
        static_cast<Fetcher *>(fetch->userData)->state_ = fetch->readyState;
    }

    std::string Fetcher::url() const {
        return fetch_->url;
    }

    bool Fetcher::isDone() const
    {
        return state_.load() == 4;
    }

    const std::byte *Fetcher::data() const
    {
        return reinterpret_cast<const std::byte *>(fetch_->data);
    }

    std::size_t Fetcher::dataSize() const
    {
        return fetch_->numBytes;
    }

}