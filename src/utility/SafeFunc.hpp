#include <charconv>
#include <string_view>

namespace Safe {
constexpr int stoi(std::string_view sv) noexcept
{
  int value = 0;
  const auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);

  if (ec != std::errc {} || ptr != sv.data() + sv.size()) {
    return 0;
  }
  return value;
}
} // namespace safe