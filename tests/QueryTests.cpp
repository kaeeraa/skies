#include "../src/utility/Parameter.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

TEST(QueryAppend, ReturnsBaseWhenNoParameters)
{
  auto queries = std::make_unique<Query::QueryVec>();
  std::string base = "/test/top";
  const std::string result = Query::append(base, std::move(queries));
  EXPECT_EQ(base, result);
}

TEST(QueryAppend, PreservesQueryParameters)
{
  auto queries = std::make_unique<Query::QueryVec>();
  queries->emplace_back("ps_args", "aux");
  queries->emplace_back("limit", "5");

  const std::string result = Query::append("/test/top", std::move(queries));
  EXPECT_TRUE(result.rfind("/test/top?", 0) == 0);
  EXPECT_NE(result.find("ps_args=aux"), std::string::npos);
  EXPECT_NE(result.find("limit=5"), std::string::npos);
}
