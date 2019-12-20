#include "Material.h"

void Material::setBool(const std::string& name, bool value)
{
  bools[name] = value;
}

void Material::setInt(const std::string& name, int value)
{
  ints[name] = value;
}

void Material::setFloat(const std::string& name, float value)
{
  floats[name] = value;
}

void Material::setVec2(const std::string& name, glm::vec2 value)
{
  vec2s[name] = value;
}

void Material::setVec3(const std::string& name, glm::vec3 value)
{
  vec3s[name] = value;
}

void Material::setVec4(const std::string& name, glm::vec4 value)
{
  vec4s[name] = value;
}

void Material::apply()
{
  shader->UseShader();

  for (auto & val: bools)
  {
    shader->setBool(val.first, val.second);
  }

  for (auto& val : ints)
  {
    shader->setInt(val.first, val.second);
  }

  for (auto& val : vec2s)
  {
    shader->setVec2(val.first, val.second);
  }

  for (auto& val : vec3s)
  {
    shader->setVec3(val.first, val.second);
  }

  for (auto& val : vec4s)
  {
    shader->setVec4(val.first, val.second);
  }

  for (auto& val : floats)
  {
    shader->setFloat(val.first, val.second);
  }

}
