#include "core/Router.hpp"
#include "core/Server.hpp"
#include "handlers/Containers.hpp"
#include "utility/Shorthands.hpp"
#include <thread>

// Consts
const auto IP = aliases::tcp::v4();
const unsigned short PORT = 8080;

int main()
{
  aliases::net::io_context ioContext;
  aliases::tcp::endpoint endpoint(IP, PORT);
  Router router;
  Docker::Handlers::Containers::Init(ioContext);

  router.get("/api/containers", Docker::Handlers::Containers::List);
  router.post("/api/containers", Docker::Handlers::Containers::Create);
  router.get("/api/containers/{id}", Docker::Handlers::Containers::Inspect);

  Server server(ioContext, endpoint, router);

  std::vector<std::jthread> threads;
  unsigned int concurrency = std::thread::hardware_concurrency();
  for (size_t i = 0; i < concurrency; ++i) {
    threads.emplace_back([&ioContext]() {
      ioContext.run();
    });
  }
}
