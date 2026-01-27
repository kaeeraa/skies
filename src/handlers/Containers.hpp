#pragma once
#include "../client/DockerClient.hpp"
#include "../utility/Shorthands.hpp"
#include <boost/asio/awaitable.hpp>

namespace Docker::Handlers {
namespace Containers {
  inline std::unique_ptr<Docker::Containers> containers;
  inline void Init(aliases::net::io_context& ioContext)
  {
    containers = std::make_unique<Docker::Containers>(ioContext.get_executor());
  }

  aliases::net::awaitable<aliases::Response> List(std::shared_ptr<const aliases::Request> raw);
  aliases::net::awaitable<aliases::Response> Create(std::shared_ptr<const aliases::Request> raw);
  aliases::net::awaitable<aliases::Response> Inspect(std::shared_ptr<const aliases::Request> raw);
  aliases::net::awaitable<aliases::Response> Top(std::shared_ptr<const aliases::Request> raw);
}
}
