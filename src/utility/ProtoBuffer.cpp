#include "ProtoBuffer.hpp"
namespace pbUtil = google::protobuf::util;

absl::Status JsonToMessage(const std::string_view json, google::protobuf::Message* message)
{
  pbUtil::JsonParseOptions options;
  options.ignore_unknown_fields = true;

  return pbUtil::JsonStringToMessage(json, message, options);
}

absl::Status JsonToMessage(const json::value& json, google::protobuf::Message* message)
{
  const std::string jsonString = json::serialize(json);
  pbUtil::JsonParseOptions options;
  options.ignore_unknown_fields = true;

  return pbUtil::JsonStringToMessage(jsonString, message, options);
}

absl::Status MessageToJson(const google::protobuf::Message& message, json::value* json)
{
  std::string jsonString;
  const absl::Status status = pbUtil::MessageToJsonString(message, &jsonString);
  *json = json::parse(jsonString);

  return status;
}