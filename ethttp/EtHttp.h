#pragma once
#include "EtHttpHeaders.h"
#include <string>

namespace EtHttp {

std::string UrlRoot(const std::string &url);
std::string UrlHost(const std::string &url);

HttpHeaders BrowserRequestHeaders(const std::string &host = {});
HttpHeaders StandardRequestHeaders();

} // namespace EtHttp
