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
  //imgui setup
  ImGui::CreateContext();
  bool show_demo_window = true;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glslVersion);

  //colors for the editor
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.22f, 0.37f, 0.94f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.60f, 1.00f, 0.40f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.60f, 0.98f, 0.67f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.17f, 0.37f, 0.69f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.28f, 0.48f, 0.77f, 1.00f);
  colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);

  //styling for the editor
  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.WindowRounding = 0.0f;
  style.WindowTitleAlign.x = 0.5f;

  ImGuiIO& io = ImGui::GetIO();
  //load fonts using "io.Fonts->AddFontFromFileTTF("your_font.ttf", size_pixels);"
}

void Editor::preRender(std::string windowName)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Editor::Render(Scene& scene, SystemManager& Manager)
{
  //*
  //learn everything here
  ImGui::ShowDemoWindow();
  //*/
  
  //if you want to be able to move the window get rid of the flag in the ImGui::Begin();
  //Inspector Window, Entity List Window
  ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoMove);

  if (ImGui::Button("Add Entity"))
  {
    default_name_count++;
    scene.addEntity("default name " + std::to_string(default_name_count));
  }

  //transform component
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
                if (v4.first == "diffuse_color")
                {
                  if (ImGui::ColorEdit3("diffuse_color", &v4.second.x))
                  {
                  }
                }
                else if (ImGui::DragFloat4(v4.first.c_str(), &v4.second.x, .1f))
                {
                }
              }

              ImGui::TreePop();
            }
          }

          if (ImGui::Button("Remove Mesh Component"))
          {
            scene.getEntities()[i]->remove<MeshComponent>();
          }

          ImGui::TreePop();
        }
      }
      else
      {
        ImGui::Separator();
        if (ImGui::Button("Add Mesh Component"))
        {
          meshComponent = scene.getEntities()[i]->add<MeshComponent>();      
          meshComponent->entity = scene.getEntities()[i];
        }
      }

      LightComponentPtr lightComponent = scene.getEntities()[i]->get<LightComponent>();
      if (lightComponent)
      {
        ImGui::Separator();
        if (ImGui::TreeNode("Light Component"))
        {
          if (ImGui::DragFloat3("Diffuse:", &lightComponent->light.diffuseColor.x, .1f, 0.0f, 1000.0f))
          {
          }

          if (ImGui::DragFloat3("Position:", &lightComponent->light.position.x, .1f))
          {
          }

          if (ImGui::DragFloat3("Specular Color:", &lightComponent->light.specularColor.x, .1f, 0.0f, 10.0f))
          {
          }

          if (ImGui::DragFloat("Specular Exponent:", &lightComponent->light.specularExponent, .1f, 0.001f, 1000.0f))
          {
          }

          if (ImGui::DragFloat("Attenuation Linear:", &lightComponent->light.linear, .1f))
          {
          }
          if (ImGui::DragFloat("Attenuation Quadratic:", &lightComponent->light.quadratic, .1f))
          {
          }

          ImGui::Separator();
          if (ImGui::Button("Remove Light Component"))
          {
            scene.getEntities()[i]->remove<LightComponent>();
          }

          ImGui::TreePop();
        }
      }
      else
      {
        ImGui::Separator();
        if (ImGui::Button("Add Light Component"))
        {
          lightComponent = scene.getEntities()[i]->add<LightComponent>();
          lightComponent->light.position = scene.getEntities()[i]->position;
        }
      }
      ImGui::Separator();

      if (ImGui::Button("Remove Entity"))
      {
        scene.removeEntity(scene.getEntities()[i]->name);
      }

    }

    ImGui::PopID();
  }


  ImGui::TextWrapped("Each Object has a light component where the light is moving with the object except for the center one.");
  ImGui::End();

  //Settings Window
  ImGui::Begin("Settings",0, ImGuiWindowFlags_NoMove);
  ImGui::TextWrapped("WASD moves the camera left, right, in, and out.");
  ImGui::Spacing();
  ImGui::TextWrapped("Q and E move up and down.");
  ImGui::Spacing();

  //render settings
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
      if (ImGui::Checkbox("uncharted tone mapping", &Manager.renderer->uncharted_tone_mapping))
      {
      }
      if (ImGui::Checkbox("tone mapping", &Manager.renderer->exposure_tone_mapping))
      {
      }

      if (Manager.renderer->exposure_tone_mapping == true)
      {
        ImGui::TextWrapped("Exposure only works for tone mapping");
        if (ImGui::DragFloat("exposure", &Manager.renderer->exposure, .1f))
        {
        }
      }

    }
    ImGui::TreePop();
  }
  ImGui::TextWrapped("Split Screen will show you what objects are using deffered rendering versus which objects are not.");
  ImGui::Spacing();

  //debug settings
  if (ImGui::CollapsingHeader("Debug Settings"))
  {
    ImGui::TreePush();
    if (Manager.debugRenderer != nullptr)
    {
      if (ImGui::Checkbox("AABB", &Manager.debugRenderer->isAABBOn))
      {
      }

      if (ImGui::Checkbox("Bounding Volume Hierarchy", &Manager.debugRenderer->isBVHOn))
      {
        Manager.debugRenderer->isAABBOn = true;
      }

      if (Manager.debugRenderer->isBVHOn)
      {
        ImGui::SliderInt("Levels", &Manager.debugRenderer->numberOfLevels, 0, 7);
      }

      if (ImGui::TreeNode("Bounding Sphere Calculation Types"))
      {
        Manager.debugRenderer->isBSOn = true;
        BoundingSphere::BoundingSphereCalculationType mode = Manager.debugRenderer->sphereType;

        if (ImGui::RadioButton("None", mode == BoundingSphere::BoundingSphereCalculationType::None))
        {
          mode = BoundingSphere::BoundingSphereCalculationType::None;
          Manager.debugRenderer->sphereType = mode;
        }
        if (ImGui::RadioButton("Centroid", mode == BoundingSphere::BoundingSphereCalculationType::Centroid))
        {
          mode = BoundingSphere::BoundingSphereCalculationType::Centroid;
          Manager.debugRenderer->sphereType = mode;
        }
        if (ImGui::RadioButton("Ritters", mode == BoundingSphere::BoundingSphereCalculationType::Ritters))
        {
          mode = BoundingSphere::BoundingSphereCalculationType::Ritters;
          Manager.debugRenderer->sphereType = mode;
        }
        if (ImGui::RadioButton("ModifiedLarsons", mode == BoundingSphere::BoundingSphereCalculationType::ModifiedLarsons))
        {
          mode = BoundingSphere::BoundingSphereCalculationType::ModifiedLarsons;
          Manager.debugRenderer->sphereType = mode;
        }
        if (ImGui::RadioButton("PCA", mode == BoundingSphere::BoundingSphereCalculationType::PCA))
        {
          mode = BoundingSphere::BoundingSphereCalculationType::PCA;
          Manager.debugRenderer->sphereType = mode;
        }

        ImGui::TreePop();
      }
      else
      {
        Manager.debugRenderer->isBSOn = false;
      }
    }

    ImGui::TreePop();
  }

  ImGui::End();

  //profiler settings
  ImGui::Begin("Profiler", 0 , ImGuiWindowFlags_NoMove);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
}


