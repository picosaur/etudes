#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Em {
using HttpHeaders = std::unordered_map<std::string, std::string>;

class HttpFetcher {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  HttpFetcher(const std::string &url, const HttpHeaders &headers = {});
  ~HttpFetcher();

  std::string url() const;
  std::string statusText() const;
  bool isDone() const;

  const std::byte *data() const;
  std::size_t dataSize() const;

  void assignData(std::string &str) const;
};
} // namespace Em