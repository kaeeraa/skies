#include <format>
#include <optional>
#include <string>
#include <string_view>

class QueryBuilder {
  public:
  QueryBuilder& add(std::string_view key, const std::optional<bool>& value)
  {
    if (value.has_value()) {
      append(key, *value ? "true" : "false");
    }
    return *this;
  }

  QueryBuilder& add(std::string_view key, const std::optional<int>& value)
  {
    if (value.has_value()) {
      append(key, std::to_string(*value));
    }
    return *this;
  }

  QueryBuilder& add(std::string_view key, const std::optional<std::string>& value)
  {
    if (value) {
      append(key, *value);
    }
    return *this;
  }

  std::string build(std::string_view base) const
  {
    if (query.empty()) {
      return std::string(base);
    }
    return std::format("{}?{}", base, query);
  }

  private:
  void append(std::string_view key, std::string_view value)
  {
    if (!query.empty()) {
      query += '&';
    }
    query += std::format("{}={}", key, value);
  }

  std::string query;
};
