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
     // ImGui::Indent();

      if (ImGui::TreeNode("Transform"))
      {
        if (ImGui::DragFloat3("Position:", &scene.getEntities()[i]->position.x, .1f))
        {
          glm::vec3& position = scene.getEntities()[i]->position;
          position.x = position.x;
          position.y = position.y;
          position.z = position.z;
        }

        if (ImGui::DragFloat3("Axis of Rotation:", &scene.getEntities()[i]->axisOfRotation.x, .1f))
        {
          glm::vec3& AoR = scene.getEntities()[i]->position;
          AoR.x = AoR.x;
          AoR.y = AoR.y;
          AoR.z = AoR.z;
        }

        if (ImGui::DragFloat("Rotation:", &scene.getEntities()[i]->angle, .1f))
        {
          float position = scene.getEntities()[i]->angle;         
        }

        if (ImGui::DragFloat3("Scale:", &scene.getEntities()[i]->scale.x, .1f))
        {
          glm::vec3& scale = scene.getEntities()[i]->scale;
          scale.x = scale.x;
          scale.y = scale.y;
          scale.z = scale.z;
        }

        ImGui::TreePop();
      }

      MeshComponentPtr meshComponent = scene.getEntities()[i]->get<MeshComponent>();
      if (meshComponent)
      {

        if (ImGui::TreeNode("Mesh Component"))
        {
          
          ImGui::TreePop();
        }
      }

      LightComponentPtr lightComponent = scene.getEntities()[i]->get<LightComponent>();
      if (lightComponent)
      {

        if (ImGui::TreeNode("Light Component"))
        {

          ImGui::TreePop();
        }
      }

      //ImGui::Unindent();
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


