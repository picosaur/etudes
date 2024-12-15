#include "EtHttp.h"
#include <stdexcept>

namespace EtHttp {

class UrlHelper {

public:
  std::string url;
  std::size_t http_pos;
  std::size_t double_slash_pos;
  std::size_t slash_pos;

  UrlHelper(const std::string &url_) : url{url_} {
    http_pos = url.find("http");
    if (http_pos == std::string::npos) {
      throw std::runtime_error("Url does not start with http");
    }
    double_slash_pos = url.find("//");
    if (double_slash_pos == std::string::npos) {
      throw std::runtime_error("Url does not contain //");
    }
    slash_pos = url.find('/', double_slash_pos + 2);
    if (slash_pos == std::string::npos) {
      slash_pos = url.length();
    }
  }

  std::string root() const { return url.substr(0, slash_pos); }

  std::string host() const {
    return url.substr(double_slash_pos + 2, slash_pos - double_slash_pos - 2);
  }
};

std::string UrlRoot(const std::string &url) {
  try {
    return UrlHelper(url).root();
  } catch (const std::exception &err) {
  }
  return {};
}

std::string UrlHost(const std::string &url) {
  try {
    return UrlHelper(url).host();
  } catch (const std::exception &err) {
  }
  return {};
}

HttpHeaders BrowserRequestHeaders(const std::string &host) {
  HttpHeaders headers{
      {"Accept",
       "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"},
      {"Accept-Encoding", "gzip, deflate, br, zstd"},
      {"Accept-Language", "en-US,en;q=0.5"},
      {"Connection", "keep-alive"},
      {"Host", host},
      {"Priority", "u=0, i"},
      {"Sec-Fetch-Dest", "document"},
      {"Sec-Fetch-Mode", "navigate"},
      {"Sec-Fetch-Site", "none"},
      {"Sec-Fetch-User", "?1"},
      {"TE", "trailers"},
      {"Upgrade-Insecure-Requests", "1"},
      {"User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; "
                     "rv:132.0) Gecko/20100101 Firefox/132.0"}};
  return headers;
}

HttpHeaders StandardRequestHeaders() {
  HttpHeaders headers{{"A-IM", "feed"},
                      {"Accept", "text/html"},
                      {"Accept-Charset", "utf-8"},
                      {"Accept-Datetime", ""},
                      {"Accept-Encoding", "gzip, deflate"},
                      {"Accept-Language", "en-US"},
                      {"Access-Control-Request-Method", "GET"},
                      {"Authorization", ""},
                      {"Cache-Control", "no-cache"},
                      {"Connection", "keep-alive"},
                      {"Content-Encoding", ""},
                      {"Content-Length", ""},
                      {"Content-MD5", ""},
                      {"Content-Type", ""},
                      {"Cookie", ""},
                      {"Date", ""},
                      {"Expect", ""},
                      {"Forwarded", ""},
                      {"From", "user@example.com"},
                      {"Host", ""},
                      {"HTTP2-Settings", ""},
                      {"If-Match", ""},
                      {"If-Modified-Since", ""},
                      {"If-None-Match", ""},
                      {"If-Range", ""},
                      {"If-Unmodified-Since", ""},
                      {"Max-Forwards", ""},
                      {"Origin", ""},
                      {"Pragma", "no-cache"},
                      {"Prefer", "return=representation"},
                      {"Proxy-Authorization", ""},
                      {"Range", ""},
                      {"Referer", ""},
                      {"TE", ""},
                      {"Trailer", ""},
                      {"Transfer-Encoding", ""},
                      {"User-Agent", "Custom Application"},
                      {"Upgrade", ""},
                      {"Via", ""},
                      {"Warning", ""}};
  return headers;
}

} // namespace EtHttp
