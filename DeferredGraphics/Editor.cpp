/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Editor.cpp
Purpose: Editor implementation with ImGui
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/

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

void Editor::Render(Scene& scene, SystemManager& Manager)
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
  ImGui::TextWrapped("Each Object has a light component where the light is moving with the object except for the center one.");
  ImGui::End();

  //Entity List Window
  ImGui::Begin("Settings");
  ImGui::TextWrapped("WASD moves the camera around the point of the center object.");
  ImGui::Spacing();
  ImGui::TextWrapped("Q and E change the FOV of the camera where Q zooms in annd E zooms out.");
  ImGui::Spacing();

  if (ImGui::CollapsingHeader("Renderer Settings"))
  {
    ImGui::TreePush();
    if (Manager.renderer != nullptr)
    {
      if (ImGui::Checkbox("Depth Toggle", &Manager.renderer->depthCopyToggle))
      {
      }

      if (ImGui::Checkbox("Split Screen", &Manager.renderer->splitScreen))
      {
      }

      if (ImGui::Checkbox("Lights On", &lightsOn))
      {
        if (lightsOn == false)
        {
          for (int i = 0; i < scene.getEntities().size(); ++i)
          {
            if (scene.getEntities().back()->get<LightComponent>() != nullptr)
            {
              scene.getEntities()[i]->get<LightComponent>()->light.diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f);
            }
          }
        }
        else
        {
          for (int i = 0; i < scene.getEntities().size(); ++i)
          {
            if (scene.getEntities().back()->get<LightComponent>() != nullptr)
            {
              scene.getEntities()[i]->get<LightComponent>()->light.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
            }
          }
        }
      }

      if (ImGui::Checkbox("gamma", &Manager.renderer->gamma))
      {
      }
      if (ImGui::Checkbox("tone mapping", &Manager.renderer->exposure_tone_mapping))
      {
      }
      if (ImGui::Checkbox("uncharted tone mapping", &Manager.renderer->uncharted_tone_mapping))
      {
      }
      if (ImGui::DragFloat("exposure", &Manager.renderer->exposure, .1f))
      {
      }

    }
    ImGui::TreePop();
  }
  ImGui::TextWrapped("Split Screen will show you what objects are using deffered rendering versus which objects are not.");
  ImGui::Spacing();
  if (ImGui::CollapsingHeader("Debug Settings"))
  {
    ImGui::TreePush();
    if (Manager.debugRenderer != nullptr)
    {
      if (ImGui::Checkbox("AABB", &Manager.debugRenderer->isAABBOn))
      {
      }
    }

    ImGui::TreePop();
  }
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


