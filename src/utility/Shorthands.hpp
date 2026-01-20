#pragma once
#include "google/protobuf/util/json_util.h"
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

namespace aliases {
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace json = boost::json;
namespace pbUtil = google::protobuf::util;
using tcp = net::ip::tcp;
using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;
using AsyncHandler = std::function<net::awaitable<Response>(std::shared_ptr<const Request>)>;
using RouteMap = std::unordered_map<std::string_view, AsyncHandler, std::hash<std::string_view>>;
} // namespace aliases
