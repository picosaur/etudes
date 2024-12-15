#if defined(ETHTTP_USE_EMSCRIPTEN)
#include "EtHttpFetcher.h"
#include <atomic>
#include <emscripten/fetch.h>
#include <stdio.h>
#include <vector>

namespace EtHttp {
class HttpFetcher::Impl {
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

  static void OnSuccess(emscripten_fetch_t *fetch) {
    printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes,
           fetch->url);
    static_cast<HttpFetcher *>(fetch->userData)->impl_->state =
        fetch->readyState;
  }

  static void OnError(emscripten_fetch_t *fetch) {
    printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url,
           fetch->status);
    static_cast<HttpFetcher *>(fetch->userData)->impl_->state =
        fetch->readyState;
  }
};

HttpFetcher::HttpFetcher(const std::string &url, const HttpHeaders &headers)
    : impl_{std::make_unique<Impl>()} {
  // copy headers
  for (const auto &header : headers) {
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
  attr.onsuccess = Impl::OnSuccess;
  attr.onerror = Impl::OnError;
  impl_->fetch = emscripten_fetch(&attr, url.c_str());
}

HttpFetcher::~HttpFetcher() {
  if (impl_->fetch != nullptr) {
    emscripten_fetch_close(impl_->fetch);
  }
}

std::string HttpFetcher::statusText() const { return impl_->fetch->statusText; }

HttpHeaders HttpFetcher::responseHeaders() const {
  return {};
}

bool HttpFetcher::isDone() const { return impl_->state.load() == 4; }

const std::byte *HttpFetcher::data() const {
  return reinterpret_cast<const std::byte *>(impl_->fetch->data);
}

std::size_t HttpFetcher::dataSize() const { return impl_->fetch->numBytes; }

void HttpFetcher::assignData(std::string &str) const {
  str.assign(impl_->fetch->data, impl_->fetch->data + impl_->fetch->numBytes);
}
} // namespace EtHttp

#endif
