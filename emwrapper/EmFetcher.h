#pragma once
#include <string>
#include <atomic>

// REFERENCES
// https://emscripten.org/docs/api_reference/fetch.html

class emscripten_fetch_t;

namespace Em
{
    class Fetcher
    {
        emscripten_fetch_t *fetch_{};

        // 0: UNSENT: request not sent yet
        // 1: OPENED: emscripten_fetch has been called.
        // 2: HEADERS_RECEIVED: emscripten_fetch has been called
        // 3: LOADING: download in progress.
        // 4: DONE: download finished.
        std::atomic_int state_{0};

    public:
        Fetcher(const std::string &url);
        ~Fetcher();

        std::string url() const; 
        bool isDone() const;

        const std::byte *data() const;
        std::size_t dataSize() const;

    private:
        static void onSuccess(emscripten_fetch_t *fetch);
        static void onError(emscripten_fetch_t *fetch);
    };
}