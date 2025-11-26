#include "client/DockerClient.hpp"
#include "core/Logger.hpp"
#include "core/Router.hpp"
#include "schema/Request.schema.hpp"
#include "utility/Json.hpp"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json/impl/serialize.hpp>
#include <boost/json/parse.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

// Shorthands
namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
using tcp       = net::ip::tcp;

// Consts
const tcp IP   = tcp::v4();
const int PORT = 8080;

// Entrypoint
int main()
{
  net::io_context    ioc;
  tcp::acceptor      acceptor(ioc, { tcp::v4(), 8080 });
  Router             router;
  Docker::Containers containers;
  const Logger&      logger = Logger::instance();

  logger.info("Logger initialized");

  router.get("/api/containers", [&containers](const Request& request) {
    json::value body = containers.list();

    Response    response { http::status::ok, request.version() };
    response.set(http::field::content_type, "application/json");
    response.body() = json::serialize(body);
    response.prepare_payload();
    return response;
  });

  router.post("/api/containers/create", [&containers](const Request& request) {
    const auto  json  = parseJson<Requests::Containers::Create>(request.body());
    json::value body = containers.create(json);

    Response    response { http::status::ok, request.version() };
    response.set(http::field::content_type, "application/json");
    response.body() = json::serialize(body);
    response.prepare_payload();
    return response;
  });

  logger.info(std::format("Server running on http://0.0.0.0:{}{}", std::to_string(PORT), "\n"));

  for (;;) {
    tcp::socket socket(ioc);
    acceptor.accept(socket);

    boost::beast::flat_buffer buffer;
    Request                   request;
    http::read(socket, buffer, request);

    auto response = router.route(request);
    http::write(socket, response);
  }
}
