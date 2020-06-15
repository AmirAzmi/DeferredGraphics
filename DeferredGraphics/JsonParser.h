#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
//forward declare the json object
class JsonObject;

//forward declare the json array
class JsonArray;

//typedefs because names get long
using JsonObjectPtr = std::unique_ptr<JsonObject>; //so i dont need to worry about deleting the pointers
using JsonArrayPtr = std::unique_ptr<JsonArray>;
using JsonValue = std::variant<std::string, double, bool, JsonObjectPtr, JsonArrayPtr>;

//objects can be within each other or other arrays
struct JsonObject
{
  std::unordered_map<std::string, JsonValue> jsonObjects;
};

//arrays can be within each other or other objects
struct JsonArray
{
  std::vector<JsonValue> jsonArrays;
};

//This defines my json types in c++ for me being able to parse them to json
////////////////////////////////////////////////////////////////////////////////

namespace jsonParser 
{
  std::string toString(const JsonValue & Val);
  JsonValue toJson(const std::string & json_data);
}

