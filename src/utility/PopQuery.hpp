#include <boost/beast/http.hpp>
#include <cstddef>
#include <string>

namespace http = boost::beast::http;
using Response = http::response<http::string_body>;
using QueryMap = std::unordered_map<std::string, std::string>;

inline QueryMap popQuery(std::string& url)
{
  QueryMap queryParams;

  size_t queryPos = url.find('?');
  if (queryPos == std::string::npos) {
    return queryParams;
  }

  std::string queryString = url.substr(queryPos + 1);
  url.erase(queryPos);

  std::istringstream queryStream(queryString);
  std::string pair;
  while (std::getline(queryStream, pair, '&')) {
    size_t eqPos = pair.find('=');
    if (eqPos != std::string::npos) {
      std::string key = pair.substr(0, eqPos);
      std::string value = pair.substr(eqPos + 1);
      queryParams[key] = value;
    } else {
      queryParams[pair] = "";
    }
  }

  return queryParams;
}
inline QueryMap popQuery(const std::string& url)
{
  QueryMap queryParams;

  size_t queryPos = url.find('?');
  if (queryPos == std::string::npos) {
    return queryParams;
  }

  std::string queryString = url.substr(queryPos + 1);

  std::istringstream queryStream(queryString);
  std::string pair;
  while (std::getline(queryStream, pair, '&')) {
    size_t eqPos = pair.find('=');
    if (eqPos != std::string::npos) {
      std::string key = pair.substr(0, eqPos);
      std::string value = pair.substr(eqPos + 1);
      queryParams[key] = value;
    } else {
      queryParams[pair] = "";
    }
  }

  return queryParams;
}