/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Editor.h
Purpose: Editor class
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Scene.h"
#include "SystemManager.h"

#include <string>

class Editor
{
  Scene& scene;
  SystemManager& manager;
public:
  ImVec2 sceneDimension;
  bool lightsOn = true;
  int default_name_count = 0;
  char buffer_size[256] = {0};
  uint8_t counter = 0;
  std::array<float, 256> arr;
  std::vector<std::string> filenames;
  std::vector<std::string> shader_filenames;
  std::vector<std::string> mesh_name;
  std::vector<ModelHandle> model_handles;
  std::vector<std::string> shader_name;
  std::vector<ShaderHandle> shader_handles;

  void init(GLFWwindow* window, const char* glslVersion);
  void preRender(std::string windowName);
  void Render(Scene & scene, SystemManager& Manager);
  void postRender();
  void shutdown();

  Editor(Scene& Scene, SystemManager Manager):scene(Scene), manager(Manager)
  {
  }
};

