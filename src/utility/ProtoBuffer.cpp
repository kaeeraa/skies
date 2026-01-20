#include "ProtoBuffer.hpp"

absl::Status JsonToMessage(const std::string_view json, google::protobuf::Message* message)
{
  aliases::pbUtil::JsonParseOptions options;
  options.ignore_unknown_fields = true;

  return aliases::pbUtil::JsonStringToMessage(json, message, options);
}

absl::Status JsonToMessage(const aliases::json::value& json, google::protobuf::Message* message)
{
  const std::string jsonString = aliases::json::serialize(json);
  aliases::pbUtil::JsonParseOptions options;
  options.ignore_unknown_fields = true;

  return aliases::pbUtil::JsonStringToMessage(jsonString, message, options);
}

absl::Status MessageToJson(const google::protobuf::Message& message, aliases::json::value* json)
{
  std::string jsonString;
  const absl::Status status = aliases::pbUtil::MessageToJsonString(message, &jsonString);
  *json = aliases::json::parse(jsonString);

  return status;
}