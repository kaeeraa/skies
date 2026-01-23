#include "core/Router.hpp"
#include "core/Server.hpp"
#include "handlers/Containers.hpp"
#include "utility/Shorthands.hpp"
#include <absl/container/inlined_vector.h>
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

  unsigned int concurrency = std::thread::hardware_concurrency();
  if (concurrency == 0) {
    concurrency = 1;
  }
  absl::InlinedVector<std::jthread, 16> threads;
  threads.reserve(concurrency);
  for (size_t i = 0; i < concurrency; ++i) {
    threads.emplace_back([&ioContext]() {
      ioContext.run();
    });
  }
}
