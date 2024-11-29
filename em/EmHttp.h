#pragma once
#include "EmHttpHeaders.h"
#include <string>

namespace Em {
namespace Http {

std::string UrlRoot(const std::string &url);
std::string UrlHost(const std::string &url);

HttpHeaders BrowserRequestHeaders(const std::string &host = {});
HttpHeaders StandardRequestHeaders();

} // namespace Http
} // namespace Em
