#include <boost/json.hpp>
#include <string_view>

namespace json = boost::json;

template <typename T>
T parseJson(std::string_view body)
{
  return json::value_to<T>(json::parse(body));
}
