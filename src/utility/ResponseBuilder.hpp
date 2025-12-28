#pragma once
#include <boost/beast/http.hpp>
#include <optional>

namespace http = boost::beast::http;
using Response = http::response<http::string_body>;

template <typename Message>
inline Response buildResponse(
  const Message& body,
  const unsigned int& version)
{
  const http::status status = body.base().has_error() ? http::status::internal_server_error : http::status::ok;
  Response response(status, version);
  response.set(http::field::content_type, "application/x-protobuf");
  response.body() = body.SerializeAsString();
  response.prepare_payload();
  return response;
}

template <typename Message>
inline Response buildResponse(
  const http::status status,
  const Message& body,
  const unsigned int& version)
{
  Response response(status, version);
  response.set(http::field::content_type, "application/x-protobuf");
  response.body() = body.SerializeAsString();
  response.prepare_payload();
  return response;
}