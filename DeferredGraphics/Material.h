#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <string>
#include "Shader.h"
#include <map>

class Material
{
  ShaderHandle shader;

  std::map<std::string, bool> bools;
  std::map<std::string, int> ints;
  std::map<std::string, float> floats;
  std::map<std::string, glm::vec2> vec2s;
  std::map<std::string, glm::vec3> vec3s;
  std::map<std::string, glm::vec4> vec4s;

  public:

  void setBool(const std::string& name, bool value);
  void setInt(const std::string& name, int value);
  void setFloat(const std::string& name, float value);
  void setVec2(const std::string& name, glm::vec2 value);
  void setVec3(const std::string& name, glm::vec3 value);
  void setVec4(const std::string& name, glm::vec4 value);

  void apply();
};


typedef std::shared_ptr<Material> MaterialHandle;