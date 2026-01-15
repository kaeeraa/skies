#include "api/v1/containers/Response.pb.h"
#include "client/DockerClient.hpp"
#include "core/Logger.hpp"
#include "core/Router.hpp"
#include "core/Server.hpp"
#include "middleware/DockerMiddleware.hpp"
#include "utility/PopQuery.hpp"
#include "utility/ResponseBuilder.hpp"
#include "utility/SafeFunc.hpp"
#include <absl/status/status.h>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/json/fwd.hpp>
#include <boost/json/impl/serialize.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value_to.hpp>
#include <filesystem>
#include <google/protobuf/util/json_util.h>
#include <iostream>
#include <string>
#include <string_view>

// Shorthands
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace containers = api::v1::containers;
using tcp = net::ip::tcp;
using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;

// Consts
const tcp IP = tcp::v4();
const int PORT = 8080;

int main()
{
  asio::io_context ioContext;
  tcp::endpoint endpoint(IP, PORT);
  Router router;
  Docker::Containers containers(ioContext.get_executor());

  router.get("/api/containers", [&containers](Request raw) -> asio::awaitable<Response> {
    containers::request::List request;
    for (const auto& [k, v] : popQuery(raw.target())) {
      if (k == "all") {
        request.set_all(v == "true");
      } else if (k == "limit") {
        request.set_limit(Safe::stoi(v));
      } else if (k == "size") {
        request.set_size(Safe::stoi(v));
      } else if (k == "filters") {
        request.set_filters(v);
      }
    }

    containers::response::List response;
    try {
      response = co_await containers.list(request);
    } catch (const std::exception& e) {
      response.mutable_data()->Clear();
      response.mutable_base()->set_error("Failed to list containers: " + std::string(e.what()));
    }

    co_return buildResponse(response, raw.version());
  });

  router.post("/api/containers", [&containers](Request raw) -> asio::awaitable<Response> {
    containers::request::Create request;
    containers::response::Create response;

    if (raw.body().empty()) {
      response.mutable_base()->set_error("Request body is empty");
      co_return buildResponse(http::status::bad_request, response, raw.version());
    }

    if (!request.ParseFromString(raw.body())) {
      response.mutable_base()->set_error("Failed to parse {Containers::Create} request");
      co_return buildResponse(response, raw.version());
    }

    try {
      response = co_await containers.create(request);
    } catch (const std::exception& e) {
      response.clear_id();
      response.mutable_base()->set_error("Failed to create container: " + std::string(e.what()));
    }

    co_return buildResponse(response, raw.version());
  });

  Server server(ioContext, endpoint, router);

  std::vector<std::thread> threads;
  auto n = std::thread::hardware_concurrency();
  for (size_t i = 0; i < n; ++i) {
    threads.emplace_back([&] { ioContext.run(); });
  }
  for (auto& t : threads) {
    t.join();
  }
}
