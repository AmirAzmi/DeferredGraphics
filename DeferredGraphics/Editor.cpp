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
    ImGui::PushID(i);
    if (ImGui::CollapsingHeader(scene.getEntities()[i]->name.c_str()))
    {
      if (ImGui::TreeNode("Transform"))
      {
        if (ImGui::DragFloat3("Position:", &scene.getEntities()[i]->position.x, .1f))
        {
        }

        if (ImGui::DragFloat3("Axis of Rotation:", &scene.getEntities()[i]->axisOfRotation.x, .1f))
        {
        }

        if (ImGui::DragFloat("Rotation:", &scene.getEntities()[i]->angle, .1f))
        {
        }

        if (ImGui::DragFloat3("Scale:", &scene.getEntities()[i]->scale.x, .1f))
        {
        }

        ImGui::TreePop();
      }

      MeshComponentPtr meshComponent = scene.getEntities()[i]->get<MeshComponent>();
      if (meshComponent)
      {
        ImGui::Separator();
        if (ImGui::TreeNode("Mesh Component"))
        {

          /*
          GLuint drawMode = meshComponent->getDrawMode();
          if (ImGui::TreeNode("Draw Mode"))
          {
            
          }
          */

          std::shared_ptr<Material> material = meshComponent->getMaterial();
          if (material)
          {
            if (ImGui::TreeNode("Material"))
            {
              for (auto& b : material->bools)
              {
                if (ImGui::Checkbox(b.first.c_str(), &b.second))
                {
                }
              }
              for (auto& v : material->floats)
              {
                if (ImGui::DragFloat(v.first.c_str(), &v.second, .1f))
                {
                }
              }

              for (auto& v2 : material->vec2s)
              {
                if (ImGui::DragFloat3(v2.first.c_str(), &v2.second.x, .1f))
                {
                }
              }

              for (auto& v3 : material->vec3s)
              {
                if (ImGui::DragFloat3(v3.first.c_str(), &v3.second.x, .1f))
                {
                }
              }

              for (auto& v4 : material->vec4s)
              {
                if (ImGui::DragFloat4(v4.first.c_str(), &v4.second.x, .1f))
                {
                }
              }

              ImGui::TreePop();
            }
          }

          std::shared_ptr<Shader> shader = meshComponent->getShader();
          if (shader)
          {
            if (ImGui::TreeNode("Shader"))
            {
              ImGui::Checkbox("Is Deffered", shader->getDeffer());
              ImGui::TreePop();
            }
          }

          ImGui::TreePop();
        }
      }

      LightComponentPtr lightComponent = scene.getEntities()[i]->get<LightComponent>();
      if (lightComponent)
      {
        ImGui::Separator();
        if (ImGui::TreeNode("Light Component"))
        {
          if (ImGui::DragFloat3("Diffuse:", &lightComponent->light.diffuseColor.x, .1f))
          {
          }

          if (ImGui::DragFloat3("Position:", &lightComponent->light.position.x, .1f))
          {
          }

          if (ImGui::DragFloat3("Specular Color:", &lightComponent->light.specularColor.x, .1f))
          {
          }

          if (ImGui::DragFloat("Specular Exponent:", &lightComponent->light.specularExponent, .1f))
          {
          }

          if (ImGui::DragFloat("Attenuation Linear:", &lightComponent->light.linear, .1f))
          {
          }
          if (ImGui::DragFloat("Attenuation Quadratic:", &lightComponent->light.quadratic, .1f))
          {
          }
          ImGui::TreePop();
        }
      }
    }

    ImGui::PopID();
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


