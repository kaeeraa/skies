#pragma once
#include <boost/describe.hpp>
#include <boost/json.hpp>

namespace Requests::Containers {
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
