#pragma once
#include <atomic>
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <optional>

namespace Requests::Containers {
struct List {
  std::optional<bool> all;
  std::optional<int>  limit;
  std::optional<bool>  size;
  std::optional<std::string> filters;
};
struct Create {
  std::string              name;
  std::string              hostname;
  std::string              domainname;
  std::string              user;
  std::vector<std::string> cmd;
  std::string              image;
};
BOOST_DESCRIBE_STRUCT(Create, (), (name, hostname, domainname, user, cmd, image))
} // namespace Requests::Containers
