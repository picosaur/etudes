#if defined(ETHTTP_USE_DUMMY)
#include "EtHttpFetcher.h"

namespace EtHttp {
class HttpFetcher::Impl {
public:
};

HttpFetcher::HttpFetcher(const std::string &url, const HttpHeaders &headers)
    : impl_{std::make_unique<Impl>()} {}

HttpFetcher::~HttpFetcher() {}

std::string HttpFetcher::statusText() const { return {}; }

HttpHeaders HttpFetcher::responseHeaders() const { return {}; }

bool HttpFetcher::isDone() const { return {}; }

const std::byte *HttpFetcher::data() const { return {}; }

std::size_t HttpFetcher::dataSize() const { return {}; }

void HttpFetcher::assignData(std::string &str) const {}
} // namespace EtHttp

#endif
