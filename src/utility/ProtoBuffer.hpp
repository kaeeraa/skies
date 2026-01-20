#pragma once
#include "Shorthands.hpp"
#include <absl/status/status.h>
#include <boost/json.hpp>
#include <google/protobuf/util/json_util.h>

absl::Status JsonToMessage(const std::string_view json, google::protobuf::Message* message);
absl::Status JsonToMessage(const aliases::json::value& json, google::protobuf::Message* message);

absl::Status MessageToJson(const google::protobuf::Message& message, aliases::json::value* json);
