#pragma once
#include <glew/GL/glew.h>
#include <glm/glm/glm.hpp>
#include <string>
#include <memory>

class Shader
{
  GLuint programID;
  bool isDeffered;

public:
  Shader(std::string vertexShader, std::string fragmentShader, bool isDeffered);
  void UseShader();
  void setBool(const std::string& name, bool value) const;
  void setInt(const std::string& name, int value) const;
  void setFloat(const std::string& name, float value) const;
  void setVec2(const std::string& name, glm::vec2 value);
  void setVec3(const std::string& name, glm::vec3 value);
  void setVec4(const std::string& name, glm::vec4 value);

  GLuint getProgramID()
  {
    return programID;
  }

  bool getDeffered()
  {
    return isDeffered;
  }

  bool * getDeffer()
  {
    return &isDeffered;
  }

};

typedef std::shared_ptr<Shader> ShaderHandle;


