#pragma once
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;