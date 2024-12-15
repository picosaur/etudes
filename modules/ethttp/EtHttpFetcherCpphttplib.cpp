#if defined(ETHTTP_USE_CPPHTTPLIB)
#include "EtHttp.h"
#include "EtHttpFetcher.h"
#include <future>
#include <httplib.h>

namespace EtHttp {

class HttpFetcher::Impl {
public:
  std::string url;
  httplib::Headers headers;
  std::future<void> future;
  std::string data;
  httplib::Response response;
  std::unique_ptr<httplib::Client> cli;

  void fetch() {
    future = std::async(std::launch::async, [this]() {
      cli = std::make_unique<httplib::Client>(UrlRoot(url));
      cli->set_connection_timeout(1, 0);
      //cli->enable_server_certificate_verification(false);
      cli->Get(
          url, headers,
          [&](const httplib::Response &response_) {
            this->response = response_;
            return true; // return 'false' if you want to cancel the request.
          },
          [&](const char *data_, size_t sz_) {
            this->data.append(data_, sz_);
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

  // copy headers
  for (const auto &header : headers) {
    impl_->headers.insert(header);
  }

  impl_->fetch();
}

HttpFetcher::~HttpFetcher() {}

bool HttpFetcher::isDone() const { return impl_->isDone(); }

std::string HttpFetcher::statusText() const { return {}; }

HttpHeaders HttpFetcher::responseHeaders() const {
  if (impl_->isDone()) {
    HttpHeaders headers;
    for (const auto &header : impl_->response.headers) {
      headers.insert(header);
    }
    return headers;
  }
  return {};
}

const std::byte *HttpFetcher::data() const {
  if (impl_->isDone()) {
    return reinterpret_cast<const std::byte *>(impl_->data.c_str());
  }
  return nullptr;
}

std::size_t HttpFetcher::dataSize() const {
  if (impl_->isDone()) {
    return impl_->data.size();
  }
  return 0;
}

void HttpFetcher::assignData(std::string &str) const {
  if (impl_->isDone()) {
    str = impl_->data;
  }
}

} // namespace EtHttp

#endif
