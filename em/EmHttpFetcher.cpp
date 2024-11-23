#include "EmHttpFetcher.h"
#include <stdio.h>

#ifdef EMSCRIPTEN

#include <emscripten/fetch.h>

namespace Em {
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

std::string HttpFetcher::url() const { return impl_->fetch->url; }

std::string HttpFetcher::statusText() const { return impl_->fetch->statusText; }

bool HttpFetcher::isDone() const { return impl_->state.load() == 4; }

const std::byte *HttpFetcher::data() const {
  return reinterpret_cast<const std::byte *>(impl_->fetch->data);
}

std::size_t HttpFetcher::dataSize() const { return impl_->fetch->numBytes; }

void HttpFetcher::assignData(std::string &str) const {
  str.assign(impl_->fetch->data, impl_->fetch->data + impl_->fetch->numBytes);
}
} // namespace Em

#else

#include <future>
#include <httplib.h>

namespace Em {
class HttpFetcher::Impl {
public:
  std::string url;
  std::future<httplib::Result> future;
  std::string body;

  std::string host() const {
    const auto http_pos = url.find("http");
    if (http_pos == std::string::npos) {
      throw std::runtime_error("Url does not start with http");
    }
    const auto double_slash_pos = url.find("//");
    if (double_slash_pos == std::string::npos) {
      throw std::runtime_error("Url does not contain //");
    }
    auto slash_pos = url.find('/', double_slash_pos + 2);
    if (slash_pos == std::string::npos) {
      slash_pos = url.length();
    }
    return url.substr(0, slash_pos);
  }

  void fetch() {
    future = std::async(std::launch::async, [this]() -> httplib::Result {
      auto cli = httplib::Client(host());
      cli.set_connection_timeout(1, 0);
      cli.enable_server_certificate_verification(false);
      return cli.Get(
          url, httplib::Headers(),
          [&](const httplib::Response &response) {
            return true; // return 'false' if you want to cancel the request.
          },
          [&](const char *data, size_t data_length) {
            body.append(data, data_length);
            return true; // return 'false' if you want to cancel the request.
          });
    });
  }

  bool isDone() {
    return future.wait_for(std::chrono::seconds(0)) ==
           std::future_status::ready;
  }
};

HttpFetcher::HttpFetcher(const std::string &url, const HttpHeaders &headers)
    : impl_{std::make_unique<Impl>()} {
  impl_->url = url;
  impl_->fetch();
}

HttpFetcher::~HttpFetcher() { }

std::string HttpFetcher::url() const { return impl_->url; }

std::string HttpFetcher::statusText() const { return {}; }

bool HttpFetcher::isDone() const { return impl_->isDone(); }

const std::byte *HttpFetcher::data() const {
  if (impl_->isDone()) {
    return reinterpret_cast<const std::byte *>(impl_->body.c_str());
  }
  return nullptr;
}

std::size_t HttpFetcher::dataSize() const {
  if (impl_->isDone()) {
    return impl_->body.size();
  }
  return 0;
}

void HttpFetcher::assignData(std::string &str) const {
  if (impl_->isDone()) {
    str = impl_->body;
  }
}

} // namespace Em

#endif