#pragma once
#include "SafeFunc.hpp"
#include <boost/beast/http.hpp>
#include <cstddef>
#include <google/protobuf/message.h>
#include <memory>
#include <string>
#include <string_view>

namespace Query {
using QueryT = std::pair<std::string_view, std::string_view>;
using QueryVec = std::vector<QueryT>;

/**
 * @brief Given a URL, parses the query string into a list of key-value pairs.
 *
 * @param url The URL to parse.
 *
 * @return A list of key-value pairs parsed from the query string.
 *
 * @example Given the URL "https://example.com/path?a=1&b=2", the function will return a list containing the pairs ("a", "1") and ("b", "2").
 */
inline std::unique_ptr<QueryVec> get(std::string_view url)
{
  auto queries = std::make_unique<QueryVec>();
  const size_t qpos = url.find('?');
  if (qpos == std::string_view::npos) {
    return queries;
  }

  std::string_view queryString = url.substr(qpos + 1);

  while (!queryString.empty()) {
    const size_t amp = queryString.find('&');
    const std::string_view pair = queryString.substr(0, amp);

    if (const size_t eq = pair.find('='); eq != std::string_view::npos) {
      queries->emplace_back(
        pair.substr(0, eq),
        pair.substr(eq + 1));
    } else {
      queries->emplace_back(pair, std::string_view {});
    }

    if (amp == std::string_view::npos) {
      break;
    }
    queryString.remove_prefix(amp + 1);
  }

  return queries;
}

/**
 * @brief Appends a query string to a given base URL.
 *
 * @param base The base URL to append the query string to.
 * @param queries A list of key-value pairs to append to the query string.
 *
 * @return A new URL with the appended query string.
 * @example Given the base URL "https://example.com/path" and the list of pairs ("a", "1") and ("b", "2"), the function will return the URL "https://example.com/path?a=1&b=2".
 */
inline std::string append(std::string_view base, std::unique_ptr<QueryVec> queries)
{
  if (queries->empty()) {
    return std::string(base);
  }

  std::ostringstream oss;
  oss << base;
  oss << '?';

  bool first = true;
  for (auto const& [k, v] : *queries) {
    if (!first) {
      oss << '&';
    }
    first = false;
    oss << k << '=' << v;
  }

  return oss.str();
}
}