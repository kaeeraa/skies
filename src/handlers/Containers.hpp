#pragma once
#include "../client/DockerClient.hpp"
#include "../utility/ResponseBuilder.hpp"
#include "../utility/Shorthands.hpp"
#include <boost/asio/awaitable.hpp>

namespace Docker::Handlers {
namespace Containers {
  inline std::unique_ptr<Docker::Containers> containers;
  inline void Init(aliases::net::io_context& ioContext)
  {
    containers = std::make_unique<Docker::Containers>(ioContext.get_executor());
  }

  template <typename Response, typename Fn>
  aliases::net::awaitable<aliases::Response>
    handleRequest(
      std::shared_ptr<const aliases::Request> raw,
      Response&& response,
      std::string logMessage,
      std::string errorPrefix,
      Fn&& fn)
  {
    try {
      Logger::instance().trace(logMessage);
      response = co_await std::forward<Fn>(fn)();
    } catch (const std::exception& e) {
      Logger::instance().error(errorPrefix + ": " + e.what());
      response.mutable_base()->set_error(errorPrefix);
    }

    co_return buildResponse(std::forward<Response>(response), raw->version());
  }

  aliases::net::awaitable<aliases::Response> List(std::shared_ptr<const aliases::Request> raw);
  aliases::net::awaitable<aliases::Response> Create(std::shared_ptr<const aliases::Request> raw);
  aliases::net::awaitable<aliases::Response> Inspect(std::shared_ptr<const aliases::Request> raw);
  aliases::net::awaitable<aliases::Response> Top(std::shared_ptr<const aliases::Request> raw);
}
}
