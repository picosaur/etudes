#pragma once
#include "EtHttpHeaders.h"
#include <memory>
#include <string>

namespace EtHttp {

class HttpFetcher {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  HttpFetcher(const std::string &url, const HttpHeaders &headers = {});
  ~HttpFetcher();

  bool isDone() const;

  std::string statusText() const;
  HttpHeaders responseHeaders() const;

  const std::byte *data() const;
  std::size_t dataSize() const;

  void assignData(std::string &str) const;
};

} // namespace EtHttp
