#include "../src/handlers/Containers.hpp"
#include "api/v1/containers/Response.pb.h"
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/use_future.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <string>

namespace {
class FakeContainers : public Docker::IContainers {
  aliases::net::awaitable<containers::response::List> list(std::unique_ptr<Query::QueryVec> queries) override
  {
    containers::response::List response;
    response.mutable_base()->Clear();
    auto* item = response.add_data();
    item->set_id("fake-id");
    co_return response;
  }

  aliases::net::awaitable<containers::response::Create> create(std::unique_ptr<containers::request::Create> request) override
  {
    co_return containers::response::Create();
  }

  aliases::net::awaitable<containers::response::Inspect> inspect(std::unique_ptr<std::string> id) override
  {
    co_return containers::response::Inspect();
  }

  aliases::net::awaitable<containers::response::Top> top(
    std::unique_ptr<std::string> id,
    std::unique_ptr<Query::QueryVec>&& queries) override
  {
    co_return containers::response::Top();
  }
};
} // namespace

TEST(ContainersHandler, ListReturnsProtoResponse)
{
  Docker::Handlers::Containers::containers = std::make_unique<FakeContainers>();

  aliases::net::io_context ioContext;
  auto request = std::make_shared<aliases::Request>();
  request->method(aliases::http::verb::get);
  request->target("/api/containers");
  request->version(11);
  request->keep_alive(false);

  auto future = aliases::net::co_spawn(
    ioContext,
    Docker::Handlers::Containers::List(request),
    aliases::net::use_future);

  ioContext.run();
  aliases::Response response = future.get();

  EXPECT_EQ(response.result(), aliases::http::status::ok);

  containers::response::List parsed;
  ASSERT_TRUE(parsed.ParseFromString(response.body()));
  ASSERT_EQ(parsed.data_size(), 1);
  EXPECT_EQ(parsed.data(0).id(), "fake-id");

  Docker::Handlers::Containers::containers.reset();
}
