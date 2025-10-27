#pragma once
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>

namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace http  = beast::http;
namespace json  = boost::json;

class DockerClient {
  public:
  // constructors
  DockerClient()
    : io()
    , socket(io)
  {
    socket.connect(asio::local::stream_protocol::endpoint("/var/run/docker.sock"));
  }

  // methods (public)
  json::value listContainers()
  {
    return request(http::verb::get, "/containers/json");
  }

  json::value createContainer(const std::string& image, const std::string& name)
  {
    json::object body;
    body["Image"] = image;
    body["name"]  = name;
    return request(http::verb::post, "/containers/create?name=" + name, body);
  }

  json::value startContainer(const std::string& id)
  {
    return request(http::verb::post, "/containers/" + id + "/start");
  }

  json::value stopContainer(const std::string& id)
  {
    return request(http::verb::post, "/containers/" + id + "/stop");
  }

  private:
  // fields (private)
  asio::io_context                     io;
  asio::local::stream_protocol::socket socket;

  // methods (private)
  json::value request(const http::verb& method, const std::string& target, const json::object& body = {});
};
