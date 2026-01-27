#pragma once
#include "../handlers/Error.hpp"
#include "../middleware/DockerMiddleware.hpp"
#include "../utility/Parameter.hpp"
#include "../utility/ProtoBuffer.hpp"
#include "api/v1/containers/Request.pb.h"
#include "api/v1/containers/Response.pb.h"
#include <boost/asio/io_context.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string_view>

namespace containers = api::v1::containers;

template <typename Resp, typename Handler = std::nullptr_t>
aliases::net::awaitable<Resp> callAndParse(
  DockerMiddleware& middleware,
  const std::string target,
  aliases::http::verb method,
  std::unique_ptr<aliases::json::object> body,
  const std::string failureContext,
  Handler&& handler = {})
{
  Resp response;
  boost::system::error_code ec;

  const aliases::json::value& raw = co_await middleware.request(method, std::move(target), std::move(body), &ec);
  if (ec) {
    setError(response, ec.message(), std::string(failureContext));
    co_return response;
  }

  if (auto objOpt = raw.try_as_object(); objOpt.has_value()) {
    const aliases::json::object& object = objOpt.value();

    if (object.empty()) {
      setError(response, "Empty response", std::string(failureContext));
      co_return response;
    }

    if (const auto& message = object.try_at("message"); message.has_value()) {
      co_return setError(response, std::format("{}: {}", failureContext, message.value().as_string().c_str()));
    }

    if constexpr (!std::is_same_v<std::remove_cvref_t<Handler>, std::nullptr_t>) {
      if (const absl::Status status = handler(object, response); !status.ok()) {
        co_return setError(response, status.ToString(), std::string(failureContext));
      }
      co_return response;
    } else {
      aliases::json::object data {
        { "data", object }
      };
      if (const absl::Status status = JsonToMessage(data, &response); !status.ok()) {
        co_return setError(response, status.ToString(), std::string(failureContext));
      }
      co_return response;
    }
  }

  if (auto arrOpt = raw.try_as_array(); arrOpt.has_value()) {
    aliases::json::object data {
      { "data", arrOpt.value() }
    };
    if (const absl::Status status = JsonToMessage(data, &response); !status.ok()) {
      co_return setError(response, status.ToString(), std::string(failureContext));
    }
    co_return response;
  }

  setError(response, "Unknown response type", std::string(failureContext));
  co_return response;
}

namespace Docker {
class Containers {
  private:
  DockerMiddleware middleware;
  aliases::net::thread_pool pool_ { std::thread::hardware_concurrency() };

  aliases::net::awaitable<containers::response::List> listUnwrapped(std::unique_ptr<Query::QueryVec> queries);
  aliases::net::awaitable<containers::response::Create> createUnwrapped(std::unique_ptr<containers::request::Create> request);
  aliases::net::awaitable<containers::response::Inspect> inspectUnwrapped(std::unique_ptr<std::string> id);
  aliases::net::awaitable<containers::response::Top> topUnwrapped(
    std::unique_ptr<std::string> id, std::unique_ptr<Query::QueryVec>&& queries);

  public:
  explicit Containers(const aliases::net::any_io_executor& ex)
    : middleware(ex)
  {
  }

  aliases::net::awaitable<containers::response::List> list(std::unique_ptr<Query::QueryVec> queries);
  aliases::net::awaitable<containers::response::Create> create(std::unique_ptr<containers::request::Create> request);
  aliases::net::awaitable<containers::response::Inspect> inspect(std::unique_ptr<std::string> id);
  aliases::net::awaitable<containers::response::Top> top(
    std::unique_ptr<std::string> id, std::unique_ptr<Query::QueryVec>&& queries);
  aliases::json::value export_(std::string_view id);
  aliases::json::value start(std::string_view id);
  aliases::json::value stop(std::string_view id);
  aliases::json::value restart(std::string_view id);
  aliases::json::value kill(std::string_view id);
  aliases::json::value update(std::string_view id, const aliases::json::object& body);
  aliases::json::value rename(std::string_view id, std::string_view name);
  aliases::json::value pause(std::string_view id);
  aliases::json::value unpause(std::string_view id);
  aliases::json::value attach(std::string_view id);
};

class Images {
  private:
  DockerMiddleware middleware;

  public:
  aliases::json::value list();
  aliases::json::value build();
  aliases::json::value prune();
  aliases::json::value create(std::string_view name, const aliases::json::object& body);
  aliases::json::value inspect();
};
}