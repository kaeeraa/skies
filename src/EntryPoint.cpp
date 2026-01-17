#include "core/Router.hpp"
#include "core/Server.hpp"
#include "handlers/Containers.hpp"
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
#include <memory>
#include <string>
#include <string_view>
#include <thread>

// Consts
const tcp IP = tcp::v4();
const int PORT = 8080;

int main()
{
  asio::io_context ioContext;
  tcp::endpoint endpoint(IP, PORT);
  Router router;
  Docker::Handlers::Containers::Init(ioContext);

  router.get("/api/containers", Docker::Handlers::Containers::List);
  router.post("/api/containers", Docker::Handlers::Containers::Create);

  Server server(ioContext, endpoint, router);

  std::vector<std::jthread> threads;
  auto n = std::thread::hardware_concurrency();
  for (size_t i = 0; i < n; ++i) {
    threads.emplace_back([&ioContext]() {
      ioContext.run();
    });
  }
}
