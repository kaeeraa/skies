#pragma once
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <exception>
#include <map>

struct BaseResponse {
  std::string                       _humansafe;
  std::optional<std::exception_ptr> error;
};
struct Port {
  int         privatePort;
  int         publicPort;
  std::string type;
};
struct Mount {
  std::string type;
  std::string name;
  std::string source;
  std::string destination;
  std::string driver;
  std::string mode;
  bool        rw;
  std::string propagation;
};
struct Health {
  std::string status;
  int         failingStreak;
};
struct List {
  std::string                        id;
  std::vector<std::string>           names;
  std::string                        image;
  std::string                        imageId;
  std::string                        imageManifestDescriptorDigest;
  std::string                        command;
  int                                created;
  std::vector<Port>                  ports;
  int                                sizeRw;
  int                                sizeRootFs;
  std::map<std::string, std::string> labels;
  std::string                        state;
  std::string                        status;
  std::map<std::string, std::string> hostConfigAnnotations;
  std::map<std::string, std::string> networkSettings;
  std::vector<Mount>                 mounts;
  Health                             health;
};

namespace Responses {
namespace Containers {
  struct List : BaseResponse {
    std::vector<List> data = {};
  };
  struct Create : BaseResponse {
    std::string id;
  };
}; // namespace Containers
}; // namespace Responses
