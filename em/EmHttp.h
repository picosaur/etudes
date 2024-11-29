#pragma once
#include "EmHttpHeaders.h"
#include <string>

namespace Em {
namespace Http {

const std::string UrlToHost(const std::string &url);

HttpHeaders GetBrowserRequestHeaders(const std::string &host = {});

HttpHeaders GetStandardHeaders();

} // namespace Http
} // namespace Em
