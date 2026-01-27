#include "../core/Logger.hpp"
#include <string_view>

template <typename T>
concept errorResponse = requires(T& response, std::string_view humanReadableError) {
  response.mutable_base();
  response.mutable_base()->Clear();
  response.mutable_base()->set_error(std::string { humanReadableError });
};

template <errorResponse T>
T& setError(
  T& response,
  std::string_view error,
  std::string_view humanReadableError) noexcept
{
  Logger::instance().error(std::format(
    "{}: {}",
    humanReadableError,
    error));

  if (auto* base = response.mutable_base(); base != nullptr) {
    base->Clear();
    base->set_error(std::string { humanReadableError });
  }

  return response;
}

template <errorResponse T>
T& setError(
  T& response,
  std::string_view error) noexcept
{
  Logger::instance().error(error);

  if (auto* base = response.mutable_base(); base != nullptr) {
    base->Clear();
    base->set_error(std::string { error });
  }

  return response;
}
