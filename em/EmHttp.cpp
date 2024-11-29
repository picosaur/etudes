#include "EmHttp.h"
#include <stdexcept>

namespace Em {
namespace Http {

const std::string UrlToHost(const std::string &url) {
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

HttpHeaders GetBrowserRequestHeaders(const std::string &host) {
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

HttpHeaders GetStandardHeaders() {
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

} // namespace Http
} // namespace Em
