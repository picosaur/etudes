#include "EmFetcher.h"
#include <emscripten/fetch.h>
#include <stdio.h>

namespace Em
{
    class Fetcher::Impl
    {
    public:
        // http request headers
        std::vector<const char *> headers;

        emscripten_fetch_t *fetch{};

        // 0: UNSENT: request not sent yet
        // 1: OPENED: emscripten_fetch has been called.
        // 2: HEADERS_RECEIVED: emscripten_fetch has been called
        // 3: LOADING: download in progress.
        // 4: DONE: download finished.
        std::atomic_int state{0};
    };

    Fetcher::Fetcher(const std::string &url, const FetcherHeaders &headers) : impl_{std::make_unique<Impl>()}
    {
        // copy headers
        for (const auto &header : headers)
        {
            impl_->headers.push_back(header.first.c_str());
            impl_->headers.push_back(header.second.c_str());
        }
        impl_->headers.push_back(nullptr);

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.userData = (this);
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.requestHeaders = impl_->headers.data();
        attr.onsuccess = Fetcher::onSuccess;
        attr.onerror = Fetcher::onError;
        impl_->fetch = emscripten_fetch(&attr, url.c_str());
    }

    Fetcher::~Fetcher()
    {
        if (impl_->fetch != nullptr)
        {
            emscripten_fetch_close(impl_->fetch);
        }
    }

    void Fetcher::onSuccess(emscripten_fetch_t *fetch)
    {
        printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
        static_cast<Fetcher *>(fetch->userData)->impl_->state = fetch->readyState;
    }

    void Fetcher::onError(emscripten_fetch_t *fetch)
    {
        printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
        static_cast<Fetcher *>(fetch->userData)->impl_->state = fetch->readyState;
    }

    std::string Fetcher::url() const
    {
        return impl_->fetch->url;
    }

    std::string Fetcher::statusText() const
    {
        return impl_->fetch->statusText;
    }

    bool Fetcher::isDone() const
    {
        return impl_->state.load() == 4;
    }

    const std::byte *Fetcher::data() const
    {
        return reinterpret_cast<const std::byte *>(impl_->fetch->data);
    }

    std::size_t Fetcher::dataSize() const
    {
        return impl_->fetch->numBytes;
    }

    void Fetcher::assign(std::string &str) const
    {
        str.assign(impl_->fetch->data, impl_->fetch->data + impl_->fetch->numBytes);
    }
}