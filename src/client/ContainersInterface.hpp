#pragma once

#include "../utility/Parameter.hpp"
#include "../utility/Shorthands.hpp"
#include "api/v1/containers/Request.pb.h"
#include "api/v1/containers/Response.pb.h"
#include <memory>

namespace Docker {
namespace containers = api::v1::containers;

class IContainers {
public:
  virtual ~IContainers() = default;

  virtual aliases::net::awaitable<containers::response::List> list(std::unique_ptr<Query::QueryVec> queries) = 0;
  virtual aliases::net::awaitable<containers::response::Create> create(std::unique_ptr<containers::request::Create> request) = 0;
  virtual aliases::net::awaitable<containers::response::Inspect> inspect(std::unique_ptr<std::string> id) = 0;
  virtual aliases::net::awaitable<containers::response::Top> top(
    std::unique_ptr<std::string> id,
    std::unique_ptr<Query::QueryVec>&& queries) = 0;
};

} // namespace Docker
