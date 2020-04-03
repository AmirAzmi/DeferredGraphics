/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.h
Purpose: Shader class for compiling shadersshaders
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

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
  void UnBindShader();
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


