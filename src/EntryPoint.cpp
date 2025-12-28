#include "api/v1/containers/Response.pb.h"
#include "client/DockerClient.hpp"
#include "core/Logger.hpp"
#include "core/Router.hpp"
#include "utility/ProtoBuffer.hpp"
#include "utility/ResponseBuilder.hpp"
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
  net::io_context ioContext;
  tcp::acceptor acceptor(ioContext, { IP, PORT });
  Router router;
  Docker::Containers containers;

  router.get("/api/containers", [&containers](const Request& raw) {
    containers::request::List request;
    containers::response::List response;

    if (const bool status = request.ParseFromString(raw.body()); !status) {
      response.mutable_base()->set_error("Failed to parse {Containers::List} request");
      return buildResponse(response, raw.version());
    }
    response = containers.list(request);
    return buildResponse(response, raw.version());
  });

  router.post("/api/containers", [&containers](const Request& raw) {
    containers::request::Create request;
    containers::response::Create response;

    if (raw.body().empty()) {
      response.mutable_base()->set_error("Request body is empty");
      return buildResponse(http::status::bad_request, response, raw.version());
    }
    if (const bool status = request.ParseFromString(raw.body()); !status) {
      response.mutable_base()->set_error("Failed to parse {Containers::Create} request");
      return buildResponse(response, raw.version());
    }
    response = containers.create(request);
    return buildResponse(response, raw.version());
  });

  std::string addressString = acceptor.local_endpoint().address().to_string();
  std::string portString = std::to_string(acceptor.local_endpoint().port());
  Logger::instance().info("Listening on " + addressString + ":" + portString);

  for (;;) {
    tcp::socket socket(ioContext);
    acceptor.accept(socket);

    beast::flat_buffer buffer;
    Request request;
    try {
      http::read(socket, buffer, request);
    } catch (const std::exception& e) {
      Logger::instance().error("Error reading request: " + std::string(e.what()));

      Response response(http::status::bad_request, request.version());
      http::write(socket, response);
      continue;
    }

    Response response = router.route(request);
    http::write(socket, response);
  }
}
