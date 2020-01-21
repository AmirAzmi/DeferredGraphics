#include "Editor.h"

void Editor::init(GLFWwindow* window, const char* glslVersion)
{
  ImGui::CreateContext();
  bool show_demo_window = true;
  ImGuiIO& io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glslVersion);
}

void Editor::preRender(std::string windowName)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  //ImGui::Begin(windowName.c_str());
}

void Editor::Render()
{
  //*
  //learn everything here
  ImGui::ShowDemoWindow();
  //*/

  //Inspector Window
  ImGui::Begin("Inspector");

  if (ImGui::CollapsingHeader("Object Name"))
  {
    ImGui::Indent();
    if (ImGui::CollapsingHeader("Components"))
    {
    }

    if (ImGui::CollapsingHeader("Components"))
    {

    }
    ImGui::Unindent();

  }

  ImGui::End();

  //Entity List Window
  ImGui::Begin("Entity List");
  ImGui::End();


  //height of main menu bar is 23 pixels just in case you were wonderings
  //creates a menu bar at the top of the window
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("Window"))
    {
      if (ImGui::MenuItem("Inspector", nullptr, false))
      {

      }
      if (ImGui::MenuItem("Entity List", nullptr, false))
      {

      }

      ImGui::EndMenu();
    }
  }
  ImGui::EndMainMenuBar();

}

void Editor::postRender()
{
  //ImGui::End();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
}


