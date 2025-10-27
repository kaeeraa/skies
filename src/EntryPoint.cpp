#include "core/Router.hpp"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
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

  router.get("/api/", [](const Request& request) {
    Response response { http::status::ok, request.version() };
    response.set(http::field::content_type, "application/json");
    response.body() = R"({"message": "!"})";
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
