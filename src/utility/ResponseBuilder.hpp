#pragma once
#include "Shorthands.hpp"
#include <boost/beast/http.hpp>
#include <optional>

template <typename Message>
inline aliases::Response buildResponse(
  const Message& body,
  const unsigned int& version)
{
  const aliases::http::status status = body.base().has_error() ? aliases::http::status::internal_server_error : aliases::http::status::ok;
  aliases::Response response(status, version);
  response.set(aliases::http::field::content_type, "application/x-protobuf");
  response.body() = body.SerializeAsString();
  response.prepare_payload();
  return response;
}

template <typename Message>
inline aliases::Response buildResponse(
  const aliases::http::status status,
  const Message& body,
  const unsigned int& version)
{
  aliases::Response response(status, version);
  response.set(aliases::http::field::content_type, "application/x-protobuf");
  response.body() = body.SerializeAsString();
  response.prepare_payload();
  return response;
}