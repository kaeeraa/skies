#pragma once
#include <absl/status/status.h>
#include <boost/json.hpp>
#include <google/protobuf/util/json_util.h>
namespace json = boost::json;

absl::Status JsonToMessage(const std::string_view json, google::protobuf::Message* message);
absl::Status JsonToMessage(const json::value& json, google::protobuf::Message* message);

absl::Status MessageToJson(const google::protobuf::Message& message, json::value* json);
