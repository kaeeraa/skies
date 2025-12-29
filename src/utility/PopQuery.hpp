#include <boost/beast/http.hpp>
#include <cstddef>
#include <string>
#include <string_view>

namespace http = boost::beast::http;
using Response = http::response<http::string_body>;
using QueryMap = std::unordered_map<std::string_view, std::string_view>;

inline QueryMap popQuery(std::string_view url)
{
  QueryMap query;

  const size_t qpos = url.find('?');
  if (qpos == std::string_view::npos) {
    return query;
  }

  std::basic_string_view<char> queryString = url.substr(qpos + 1);

  while (!queryString.empty()) {
    const size_t amp = queryString.find('&');
    const std::basic_string_view<char> pair = queryString.substr(0, amp);

    if (const size_t eq = pair.find('='); eq != std::string_view::npos) {
      query.try_emplace(
        pair.substr(0, eq),
        pair.substr(eq + 1));
    } else {
      query.try_emplace(pair, std::string_view {});
    }

    if (amp == std::string_view::npos) {
      break;
    }
    queryString.remove_prefix(amp + 1);
  }

  return query;
}