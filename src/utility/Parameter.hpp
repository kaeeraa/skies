#pragma once
#include "../core/Logger.hpp"
#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>
#include <algorithm>
#include <boost/beast/http.hpp>
#include <cstddef>
#include <google/protobuf/message.h>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>

namespace Query {
using QueryT = std::pair<std::string_view, std::string_view>;
using QueryVec = absl::InlinedVector<QueryT, 8>;

/**
 * @brief Removes the query string from a given URL.
 *
 * If the URL contains a query string (i.e., it contains a '?'), this function will remove
 * the query string from the URL by erasing all characters from the first '?' to the end of
 * the string.
 *
 * @param url The URL to remove the query string from.
 * @example Given the URL "https://example.com/path?a=1&b=2", the function will modify the URL to "https://example.com/path".
 */
inline void remove(std::string& url)
{
  if (auto it = std::ranges::find(url, '?'); it != url.end()) {
    url.erase(it, url.end());
  }
}

/**
 * @brief Given a URL, parses the query string into a list of key-value pairs.
 *
 * @param url The URL to parse.
 *
 * @return A list of key-value pairs parsed from the query string.
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

namespace Path {
using PathParams = absl::flat_hash_map<std::string, std::string>;

/**
 * @brief Splits a given string into segments separated by '/'.
 * @attention Does not handle more than 16 segments. If the input string contains more than 16 segments, returns an empty vector.
 *
 * @param url The input string to split.
 *
 * @return A vector of string views, each representing a segment of the input string.
 * @example Given the input string "a/b/c", the function will return a vector containing the segments "a", "b", and "c".
 */
inline absl::InlinedVector<std::string, 16> splitSlash(const std::string_view url)
{
  unsigned short count = std::ranges::count(url, '/');
  if (count > 16) {
    Logger::instance().error(std::format("Too long path, URL: {}", url));
    return {};
  }

  absl::InlinedVector<std::string, 16> segments;
  segments.reserve(count + 1);

  size_t pos = 0;
  while (pos < url.size()) {
    size_t next = url.find('/', pos);
    if (next == std::string_view::npos) {
      next = url.size();
    }
    if (next > pos) {
      if (url[next - 1] == '/') {
        next--;
      }
    }
    segments.emplace_back(url.substr(pos, next - pos));
    pos = next + 1;
  }
  return segments;
}
/**
 * @brief Given a URL and a pattern, parses the URL into a list of key-value pairs based on the pattern.
 *
 * @param url The URL to parse.
 * @param pattern The pattern to parse the URL against.
 * @return A list of key-value pairs, where each key is a parameter name and each value is the parameter's value.
 * @example Given the URL "https://example.com/path/a/b" and the pattern "https://example.com/path/{a}/{b}", the function will return a list containing the pairs ("a", "a") and ("b", "b").
 */
inline PathParams get(std::string_view url, const std::string_view pattern)
{
  const size_t queryPos = url.find('?');
  if (queryPos != std::string_view::npos) {
    url.remove_suffix(url.size() - queryPos);
  }

  auto urlSegments = splitSlash(url);
  auto patternSegments = splitSlash(pattern);

  if (urlSegments.size() != patternSegments.size()) {
    return {};
  }

  PathParams params;
  for (auto [patternSegment, urlSegment] : std::views::zip(patternSegments, urlSegments)) {
    if (!patternSegment.empty() && patternSegment.front() == '{' && patternSegment.back() == '}') {
      std::string name = patternSegment.substr(1, patternSegment.size() - 2);
      params[name] = urlSegment;
      Logger::instance().debug(std::format("Found parameter '{}' with value '{}'", name, urlSegment));
    } else if (patternSegment != urlSegment) {
      Logger::instance().error(
        std::format("Path mismatch, URL segment: '{}', Pattern segment: '{}'",
                    urlSegment, patternSegment));
      return {};
    }
  }

  return params;
}
/**
 * @brief Verifies if a given URL matches a given pattern.
 *
 * @param url The URL to verify against the pattern.
 * @param pattern The pattern to verify against.
 * @return True if the URL matches the pattern, false otherwise.
 * @example Given the URL "https://example.com/path/a/b" and the pattern "https://example.com/path/{a}/{b}", the function will return true.
 */
inline bool verify(std::string_view url, std::string_view pattern) noexcept
{
  auto urlSegments = splitSlash(url);
  auto patternSegments = splitSlash(pattern);

  if (urlSegments.size() != patternSegments.size()) {
    return false;
  }

  for (size_t i = 0; i < urlSegments.size(); ++i) {
    auto& patternSegment = patternSegments[i];
    if (patternSegment.size() >= 2 && patternSegment.front() == '{' && patternSegment.back() == '}') {
      continue;
    }
    if (patternSegment != urlSegments[i]) {
      return false;
    }
  }

  return true;
}
}