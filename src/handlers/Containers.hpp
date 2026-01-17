#pragma once
#include "../client/DockerClient.hpp"
#include "../utility/ResponseBuilder.hpp"
#include "../utility/Shorthands.hpp"
#include <boost/asio/awaitable.hpp>

namespace Docker::Handlers {
namespace Containers {
  inline std::unique_ptr<Docker::Containers> containers;
  inline void Init(asio::io_context& ioContext)
  {
    containers = std::make_unique<Docker::Containers>(ioContext.get_executor());
  }

  net::awaitable<Response> List(std::shared_ptr<const Request> raw);
  asio::awaitable<Response> Create(std::shared_ptr<const Request> raw);
}
}
