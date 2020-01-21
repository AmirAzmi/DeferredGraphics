#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"

#include <string>

class Editor
{
public:
  void init(GLFWwindow* window, const char* glslVersion);
  void preRender(std::string windowNmae);
  void Render();
  void postRender();
  void shutdown();




};

