#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Scene.h"

#include <string>

class Editor
{
  Scene& scene;
public:
  void init(GLFWwindow* window, const char* glslVersion);
  void preRender(std::string windowNmae);
  void Render(Scene & scene);
  void postRender();
  void shutdown();

  Editor(Scene& Scene):scene(Scene)
  {

  }




};

