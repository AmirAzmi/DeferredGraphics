/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Material.h
Purpose: Material class for managing the uniforms passed in
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
#include <map>
#include "Shader.h"

class Material
{
public:
  ShaderHandle shader;

  std::map<std::string, bool> bools;
  std::map<std::string, int> ints;
  std::map<std::string, float> floats;
  std::map<std::string, glm::vec2> vec2s;
  std::map<std::string, glm::vec3> vec3s;
  std::map<std::string, glm::vec4> vec4s;
  std::map<std::string, glm::mat4> mat4s;

  public:

    Material(ShaderHandle shader):shader(shader)
    {

    }

  void setBool(const std::string& name, bool value);
  void setInt(const std::string& name, int value);
  void setFloat(const std::string& name, float value);
  void setVec2(const std::string& name, glm::vec2 value);
  void setVec3(const std::string& name, glm::vec3 value);
  void setVec4(const std::string& name, glm::vec4 value);
  void setMat4(const std::string& name, glm::mat4 value);

  void apply();
};


typedef std::shared_ptr<Material> MaterialHandle;