#include "Editor.h"
#include "Entity.h"

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

void Editor::Render(Scene& scene)
{
  //*
  //learn everything here
  ImGui::ShowDemoWindow();
  //*/

  //Inspector Window
  ImGui::Begin("Inspector");

  for (int i = 0; i < scene.getEntities().size(); ++i)
  {

    if (ImGui::CollapsingHeader(scene.getEntities()[i]->name.c_str()))
    {
      ImGui::Indent();

      
      if (ImGui::TreeNode("Transform"))
      {
        if (ImGui::DragFloat3("Scale Value:", &scene.getEntities()[i]->scale.x, .2f))
        {
          glm::vec3& scale = scene.getEntities()[i]->scale;
          scale.x = scale.x;
          scale.y = scale.y;
          scale.z = scale.z;
        }

        ImGui::TreePop();
      }
      

      if (scene.getEntities()[i] != nullptr)
      {

        if (ImGui::TreeNode("Mesh Component"))
        {
          
          ImGui::TreePop();
        }
      }

      if (scene.getLights()[i] != nullptr)
      {
        if (ImGui::TreeNode("Light Component"))
        {

          ImGui::TreePop();
        }
      }

      ImGui::Unindent();

    }
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


