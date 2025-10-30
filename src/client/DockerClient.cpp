#include "DockerClient.hpp"
#include "../middleware/DockerMiddleware.hpp"
#include <format>
#include <string>

json::value Docker::Containers::list()
{
  return middleware.request(http::verb::get, "/containers/json");
}

json::value Docker::Containers::create(std::string_view name, const json::object& body)
{
  const std::string target = std::format("/containers/create?name={}", name);
  return middleware.request(http::verb::post, target, body);
}

json::value Docker::Containers::inspect(std::string_view id)
{
  const std::string target = std::format("/containers/{}/json?size=1", id);
  return middleware.request(http::verb::get, target);
}

json::value Docker::Containers::processes(std::string_view id)
{
  const std::string target = std::format("/containers/{}/top", id);
  return middleware.request(http::verb::get, target);
}

json::value Docker::Containers::export_(std::string_view id)
{
  const std::string target = std::format("/containers/{}/export", id);
  // TODO: implement export using tarball
  return {};
}

json::value Docker::Containers::start(std::string_view id)
{
  const std::string target = std::format("/containers/{}/start", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::stop(std::string_view id)
{
  const std::string target = std::format("/containers/{}/stop", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::restart(std::string_view id)
{
  const std::string target = std::format("/containers/{}/restart", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::kill(std::string_view id)
{
  const std::string target = std::format("/containers/{}/kill", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::update(std::string_view id, const json::object& body)
{
  const std::string target = std::format("/containers/{}/update", id);
  return middleware.request(http::verb::post, target, body);
}

json::value Docker::Containers::rename(std::string_view id, std::string_view name)
{
  const std::string target = std::format("/containers/{}/rename?name={}", id, name);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::pause(std::string_view id)
{
  const std::string target = std::format("/containers/{}/pause", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::unpause(std::string_view id)
{
  const std::string target = std::format("/containers/{}/unpause", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::attach(std::string_view id)
{
  const std::string target = std::format("/containers/{}/attach/ws", id);
  // TODO: implement attaching via websocket
  return {};
}