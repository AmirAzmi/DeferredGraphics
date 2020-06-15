#include "JsonParser.h"

std::string jsonParser::toString(const JsonValue& Val)
{
  
}


void skipWhitespaces(const std::string& json_data, size_t& current_location)
{
  //while its not a whitespace and valid
  while (std::isspace(json_data[current_location]) && current_location < json_data.size())
  {
    current_location++; //increment current location
  }
}

JsonValue jsonSolver(const std::string& json_data, size_t& current_location)
{
  //skip all whitespaces
  skipWhitespaces(json_data, current_location);

  switch (json_data[current_location])
  {
    //this means we are at the beginning of an object
    case '{':
    {
      //create unique pointer
      JsonObjectPtr jsonObj = std::make_unique<JsonObject>();

      //move over the curly brace
      current_location++;

      //go to string within object
      while (json_data[current_location] != '}' && current_location < json_data.size())
      {
        JsonValue key = jsonSolver(json_data, current_location);
        skipWhitespaces(json_data, current_location);

        if (json_data[current_location] == ':')
        {
          current_location++;
        }

        JsonValue value = jsonSolver(json_data, current_location);
        skipWhitespaces(json_data, current_location);

        if (json_data[current_location] == ',')
        {
          current_location++;
        }

        skipWhitespaces(json_data, current_location);

        if (std::holds_alternative<std::string>(key))
        {
          std::string keyStr = std::get<std::string>(key);
          jsonObj->jsonObjects[keyStr] = std::move(value);
        }

      }

      if (json_data[current_location] == '}')
      {
        current_location++;
      }

      return jsonObj;
    }

    //this means we are at the beginning of an array
    case '[':
    {
      //create unique pointer
      JsonArrayPtr jsonArr = std::make_unique<JsonArray>();

      //move over the bracket
      current_location++;

      //go to string within object
      while (json_data[current_location] != ']' && current_location < json_data.size())
      {
        JsonValue value = jsonSolver(json_data, current_location);
        skipWhitespaces(json_data, current_location);
       
        if (json_data[current_location] == ',')
        {
          current_location++;
        }

        skipWhitespaces(json_data, current_location);
        jsonArr->jsonArrays.emplace_back(std::move(value));
      }

      if (json_data[current_location] == ']')
      {
        current_location++;
      }

      return jsonArr;
    }

    //this means we are at the beginning of an string
    case '\"':
    {
      current_location++;

      size_t start_pos = current_location;
      while (json_data[current_location] != '\"' && current_location < json_data.size())
      {
        current_location++;

      }

      std::string json_str = json_data.substr(start_pos, current_location - start_pos);

      if (json_data[current_location] == '\"')
      {
        current_location++;
      }

      return json_str;
    }

    //this means we are at the beginning of a null
    case 'n':
    {
      current_location += 4; //null
      return {};
      break;
    }

    case 't':
    {
      current_location += 4; //true
      return true;
      break;
    }

    case 'f':
    {
      current_location += 5; //false
      return false;
      break;
    }

    default:
    {
      const char * start_index = json_data.c_str() + current_location;
      char* end_index;
      double value = std::strtod(json_data.c_str() + current_location, &end_index);
      current_location += end_index - start_index;
      return value;
    }
  }

}


JsonValue jsonParser::toJson(const std::string& json_data)
{
  size_t current_location = 0;
  return jsonSolver(json_data, current_location);
}

