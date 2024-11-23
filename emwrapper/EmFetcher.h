#pragma once
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <vector>

// REFERENCES
// https://emscripten.org/docs/api_reference/fetch.html

class emscripten_fetch_t;

namespace Em
{
    using FetcherHeaders = std::unordered_map<std::string, std::string>;

    class Fetcher
    {
        class Impl;
        std::unique_ptr<Impl> impl_;

    public:
        Fetcher(const std::string &url, const FetcherHeaders &headers = {});
        ~Fetcher();

        std::string url() const; 
        std::string statusText() const;
        bool isDone() const;

        const std::byte *data() const;
        std::size_t dataSize() const;

        void assign(std::string &str) const;

    private:
        static void onSuccess(emscripten_fetch_t *fetch);
        static void onError(emscripten_fetch_t *fetch);
    };
}