#include "core/Router.hpp"
#include "middleware/DockerClient.hpp"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json/impl/serialize.hpp>
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
  net::io_context ioc;
  tcp::acceptor   acceptor(ioc, { tcp::v4(), 8080 });
  Router          router;
  DockerClient    docker;

  router.get("/api/containers", [&docker](const Request& request) {
    json::value containers = docker.listContainers();

    Response    response { http::status::ok, request.version() };
    response.set(http::field::content_type, "application/json");
    response.body() = json::serialize(containers);
    response.prepare_payload();
    return response;
  });

  std::cout << std::format("Server running on http://0.0.0.0:{}{}", std::to_string(PORT), "\n");

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
