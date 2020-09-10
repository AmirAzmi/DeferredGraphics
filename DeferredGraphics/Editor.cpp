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
#include <filesystem> //filenames
#include <iostream>
#include "Memory.h"
#include "Raycast.h"
#include <Imgui/imgui_internal.h>
#include "Behavior.h"

float lerp(float a, float b, float t)
{
  return (1.0 - t) * a + b * t;
}

float invLerp(float a, float b, float v)
{
  return  (v - a) / (b - a);
}

float remap(float min, float max, float imin, float imax, float v)
{
  float t = invLerp(min, max, v);
  return lerp(imin, imax, t);
}


namespace fs = std::filesystem;
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
  colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.45f, 0.71f, 0.94f);
  colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.11f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.68f, 0.68f, 0.72f, 1.00f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.24f, 0.40f, 0.57f, 1.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.17f, 0.33f, 0.67f, 0.07f);

  //styling for the editor
  ImGuiStyle& style = ImGui::GetStyle();
  style.FrameRounding = 3.0f;
  style.ChildRounding = 3.0f;
  style.ScrollbarRounding = 3.0f;
  style.WindowRounding = 3.0f;
  style.WindowTitleAlign.x = 0.5f;
  style.WindowTitleAlign.y = 0.5f;


  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  //load fonts using "io.Fonts->AddFontFromFileTTF("your_font.ttf", size_pixels);"

  //This next set of code is used to generate all mesh handles from the resources 
  //folder for swapping mehes in realtimechecks current path folder which contains the obj files
  std::string res = "Resources";
  std::string shader_folder = "Resources/Shaders";

  //search through the current path within resources
  for (const auto& entry : fs::directory_iterator(res))
  {
    if (entry.is_regular_file())
    {
      filenames.push_back(entry.path().string());
    }
  }

  //gets the file from the filename and stores it into the mesh_names
  for (auto& file : filenames)
  {
    std::size_t iterator = file.find_last_of("/\\");
    mesh_name.push_back(res + "/" + file.substr(iterator + 1));
  }

  //contains the correct path names
  for (int i = 0; i < mesh_name.size(); ++i)
  {
    std::cout << mesh_name[i] << std::endl;
    MeshHandle mesh = std::make_shared<Mesh>(mesh_name[i]);
    mesh_handles.push_back(mesh);
  }

  //reuse this buffer 
  mesh_name.clear();
  for (auto& file : filenames)
  {
    std::size_t iterator = file.find_last_of("/\\");
    mesh_name.push_back(file.substr(iterator + 1));
  }

  //search throughthe current path within shaders
  for (const auto& entry : fs::directory_iterator(shader_folder))
  {
    shader_filenames.push_back(entry.path().string());
  }

  //gets the file from the filename and stores it into the shader_names
  for (auto& file : shader_filenames)
  {
    std::size_t iterator = file.find_last_of("/\\");
    shader_name.push_back(shader_folder + "/" + file.substr(iterator + 1));
  }

  //contains the correct path names for the shader
  for (int i = 0; i < shader_name.size(); i += 2)
  {
    std::cout << shader_name[i] << std::endl;
    ShaderHandle shader = std::make_shared<Shader>(shader_name[i + 1], shader_name[i], true);
    shader_handles.push_back(shader);
  }

  //set names in editor by resusing the shader name buffer
  shader_name.clear();
  for (auto& file : shader_filenames)
  {
    std::size_t iterator = file.find_last_of("/\\");
    shader_name.push_back(file.substr(iterator + 1));
  }
}

void Editor::preRender(std::string windowName)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static bool opt_fullscreen_persistant = true;
  bool opt_fullscreen = opt_fullscreen_persistant;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
  // because it would be confusing to have two docking targets within each others.
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen)
  {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
  // and handle the pass-thru hole, so we ask Begin() to not render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
  // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
  // all active windows docked into it will lose their parent and become undocked.
  // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
  // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", 0, window_flags);
  ImGui::PopStyleVar();

  if (opt_fullscreen)
    ImGui::PopStyleVar(2);

  // DockSpace
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }

  ImGui::End();
}

void Editor::Render(Scene& scene, SystemManager& Manager)
{
  //*
  //learn everything here
  ImGui::ShowDemoWindow();
  //*/

  //highlight/manage objects here
  //raycast from cursor position into the screen
  //for any intersections of BVs, check the leaf nodes thoroughly
  //if you want to be able to move the window get rid of the flag in the ImGui::Begin();
  //Inspector Window, Entity List Window
  ImGui::Begin("Inspector");
  if (ImGui::InputText("Search", buffer_size, sizeof(buffer_size)))
  {
    int size = std::strlen(buffer_size);

    for (int i = 0; i < size; ++i)
    {
      buffer_size[i] = std::tolower(buffer_size[i]);
    }
  }

  std::string search_word(buffer_size);//cast char * to string

  if (!search_word.empty())
  {
    ImGui::SameLine(); //this is for putting the clear on the same line when there are
    //words in the search bar

    if (ImGui::Button("Clear"))
    {
      buffer_size[0] = '\0'; //to clear the string the first element is null terminated
    }
  }

  if (ImGui::Button("Add Entity"))
  {
    default_name_count++;
    scene.addEntity("default name " + std::to_string(default_name_count));
  }

  //transform component
  for (int i = 0; i < scene.getEntities().size(); ++i)
  {
    const std::string& name = scene.getEntities()[i]->name;

    if (!search_word.empty())
    {
      std::string lower_str;

      for (int j = 0; j < name.size(); ++j)
      {
        lower_str.push_back(std::tolower(name[j]));
      }

      if (lower_str.find(search_word) == std::string::npos)
      {
        continue;
      }
    }

    ImGui::PushID(i);
    if (ImGui::CollapsingHeader(scene.getEntities()[i]->name.c_str()))
    {
      ImGui::Indent();
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
          //second parameter can be the mesh name if i had a mesh name id 
          if (ImGui::BeginCombo("Mesh List", " + Mesh"))
          {
            for (int i = 0; i < mesh_name.size(); ++i)
            {
              if (ImGui::Selectable(mesh_name[i].c_str()))
              {
                meshComponent->mesh = mesh_handles[i];
                meshComponent->vertices = meshComponent->getVec4Vertices();
              }
            }

            ImGui::EndCombo();
          }

          //add changing of mesh here
          if (ImGui::BeginCombo("Shader List", " + Shader"))
          {
            for (int i = 0; i < shader_name.size(); i += 2)
            {
              if (ImGui::Selectable(shader_name[i].c_str()))
              {
                meshComponent->shader = shader_handles[i];
              }
            }

            ImGui::EndCombo();
          }

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
                  if (ImGui::ColorEdit3("Diffuse Color", &v4.second.x))
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
          meshComponent->vertices = meshComponent->getVec4Vertices();
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

      std::vector<BehaviorPtr> behaviorList = scene.getEntities()[i]->behaviors;

      ImGui::Separator();
      if (ImGui::TreeNode("Behaviors"))
      {
        if (!behaviorList.empty())
        {
          for (auto& behave : behaviorList)
          {
            if (ImGui::CollapsingHeader(behave->name().c_str()))
            {
              behave->inspect();
            }
          }
        }

        ImGui::TreePop();
      }

      ImGui::Separator();

      if (ImGui::Button("Remove Entity"))
      {
        scene.removeEntity(scene.getEntities()[i]->name);
      }
      ImGui::Unindent();
    }
    ImGui::PopID();
  }
  ImGui::End();

  //Settings Window
  ImGui::Begin("Settings");
  ImGui::TextWrapped("WASD moves the camera left, right, in, and out.");
  ImGui::Spacing();
  ImGui::TextWrapped("Q and E move up and down.");
  ImGui::Spacing();
  ImGui::TextWrapped("Screen Space Mouse Position. X: %.2f", ImGui::GetIO().MousePos.x);
  ImGui::TextWrapped("Screen Space Mouse Position. Y: %.2f", ImGui::GetIO().MousePos.y);
  ImGui::Spacing();

  ImGui::Text("Camera Settings");
  if (ImGui::DragFloat("Camera Speed", &scene.cameraSpeed, .05f))
  {
  }

  if (ImGui::DragFloat("Camera FOV", &scene.fov, .1f))
  {
  }

  if (ImGui::DragFloat3("Camera Eye Position", &scene.eyePosition.x, 0.05f))
  {
  }

  if (ImGui::DragFloat3("Camera Up Direction", &scene.upDirection.x, 0.05f))
  {
  }

  if (ImGui::DragFloat3("Camera Direction", &scene.cameraDirection.x, 0.05f))
  {
  }

  //render settings
  if (ImGui::CollapsingHeader("Renderer Settings"))
  {
    ImGui::Indent();
    if (Manager.renderer != nullptr)
    {
      if (ImGui::Checkbox("Depth Toggle", &Manager.renderer->depthCopyToggle))
      {
      }

      if (ImGui::Checkbox("Split Screen", &Manager.renderer->splitScreen))
      {
      }

      if (ImGui::Checkbox("Bright Buffer", &Manager.renderer->brightBuffer))
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

      if (ImGui::Checkbox("Bloom", &Manager.renderer->bloom))
      {
      }

      if (ImGui::Checkbox("Uncharted Tone Mapping", &Manager.renderer->uncharted_tone_mapping))
      {
      }

      if (ImGui::Checkbox("Tone Mapping", &Manager.renderer->exposure_tone_mapping))
      {
      }

      if (Manager.renderer->exposure_tone_mapping == true)
      {
        ImGui::TextWrapped("Exposure only works for tone mapping");
        if (ImGui::DragFloat("Exposure", &Manager.renderer->exposure, .05f))
        {
        }
      }

      if (ImGui::Checkbox("Gamma Correction", &Manager.renderer->gamma))
      {
      }


    }
    ImGui::Unindent();
  }
  ImGui::TextWrapped("Split Screen will show you what objects are using deffered rendering versus which objects are not.");
  ImGui::Spacing();

  //debug settings
  if (ImGui::CollapsingHeader("Debug Settings"))
  {
    ImGui::Indent();
    if (Manager.debugRenderer != nullptr)
    {
      if (ImGui::Checkbox("AABB", &Manager.debugRenderer->isAABBOn))
      {
      }

      if (ImGui::Checkbox("Square AABB", &Manager.debugRenderer->isSquareAABBOn))
      {
      }

      ImGui::TextWrapped("Levels are color coded where the deepest level is always the level color. Lowest level color is: Black");
      if (ImGui::Checkbox("Octree For Center Object", &Manager.debugRenderer->isSubObjectDrawOn))
      {
        Manager.debugRenderer->isAABBOn = true;
      }

      if (Manager.debugRenderer->isSubObjectDrawOn)
      {
        ImGui::SliderInt("Octree Levels", &Manager.debugRenderer->levelForOneObject, 0, 7);
      }


      if (ImGui::Checkbox("Bounding Volume Hierarchy Top Down", &Manager.debugRenderer->isBVHOn))
      {
        Manager.debugRenderer->isAABBOn = true;
      }

      if (Manager.debugRenderer->isBVHOn)
      {
        ImGui::SliderInt("Levels: Top Down", &Manager.debugRenderer->numberOfLevels, 0, 7);
      }

      if (ImGui::Checkbox("Bounding Volume Hierarchy Bottom Up", &Manager.debugRenderer->isBVHBottomUpOn))
      {
        Manager.debugRenderer->isAABBOn = true;
      }

      if (Manager.debugRenderer->isBVHBottomUpOn)
      {
        ImGui::SliderInt("Levels: Bottom Up", &Manager.debugRenderer->numberOfLevelsBUOn, 0, 7);
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
    ImGui::Unindent();
  }

  ImGui::End();

  {
    //profiler settings
    ImGui::Begin("Profiler");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (ImGui::BeginTable("Table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
    {

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Temp Memory Usage");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Current Mem Used");
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Octree Temp Mem Usage:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(" %i / %i", Manager.debugRenderer->memory_usage_from_octree, linearAllocator.memory_size);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("BVH TD Temp Mem Usage:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(" %i / %i", Manager.debugRenderer->memory_usage_from_BVHTopeDown, linearAllocator.memory_size);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Total Mem Usage:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(" %i / %i", Manager.debugRenderer->memory_usage_from_BVHTopeDown + Manager.debugRenderer->memory_usage_from_octree, linearAllocator.memory_size);
      ImGui::EndTable();
    }

    if (ImGui::BeginTable("Table2", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Debug Function Draw Calls");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Current # of Draws");

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("DebugRendering::DrawOctree:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("# of draws %i", Manager.debugRenderer->octree_draw_calls);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("DebugRendering::DrawBVHTopDown:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("# of draws %i", Manager.debugRenderer->bvh_draw_calls);
      ImGui::EndTable();
    }


    if (ImGui::BeginTable("Table3", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Systems");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("Time in milliseconds");

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Debug Rendering System Frame Time:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f ms/frame (%.1f / 100.0)", Manager.debugRenderer->rendering_sytem_elapsed_time,
        ((Manager.debugRenderer->rendering_sytem_elapsed_time) / (1000.0f / ImGui::GetIO().Framerate)) * 100.0f);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Main Rendering System Frame Time:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f ms/frame (%.1f / 100.0)", Manager.renderer->rendering_sytem_elapsed_time,
        ((Manager.renderer->rendering_sytem_elapsed_time) / (1000.0f / ImGui::GetIO().Framerate)) * 100.0f);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Total Rendering Systems Frame Time:");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%f ms/frame (%.1f / 100.0)", Manager.renderer->rendering_sytem_elapsed_time + Manager.renderer->rendering_sytem_elapsed_time,
        ((Manager.renderer->rendering_sytem_elapsed_time + Manager.debugRenderer->rendering_sytem_elapsed_time) / (1000.0f / ImGui::GetIO().Framerate)) * 100.0f);
      ImGui::EndTable();
    }

    ImGui::End();
  }

  //window settings
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Scene");
  ImVec2 window_size = ImGui::GetContentRegionAvail();
  ImVec2 window_pos = ImGui::GetCursorScreenPos();
  sceneDimension = window_pos + window_size;

  glm::vec4 screen_space_cursor;
  ImVec2 curs(ImGui::GetIO().MousePos);
  screen_space_cursor.x = ImGui::GetIO().MousePos.x;
  screen_space_cursor.y = ImGui::GetIO().MousePos.y;
  screen_space_cursor.z = -1.0f;
  screen_space_cursor.w = 1.0f;

  //if its within the window bounds, calcualte world pos
  /*if (screen_space_cursor.x >= window_pos.x &&
    screen_space_cursor.y >= window_pos.y &&
    screen_space_cursor.x <= window_pos.x + window_size.x &&
    screen_space_cursor.y <= window_pos.y + window_size.y)
  {
    //std::cout << "\nScreen Space X: " << screen_space_cursor.x << "\n";
    //std::cout << "Screen Space Y: " << screen_space_cursor.y << "\n";

    screen_space_cursor.x = remap(window_pos.x, window_pos.x + window_size.x, -1.0f, 1.0f, curs.x);
    screen_space_cursor.y = remap(window_pos.y, window_pos.y + window_size.y, 1.0f, -1.0f, curs.y);

    //std::cout << "\nNDC Space X: " << screen_space_cursor.x << "\n";
    //std::cout << "NDC Space Y: " << screen_space_cursor.y << "\n";
    //-----------------------------------------------------------------------------
    glm::mat4 persp = scene.getProjectionMatrix();
    glm::mat4 view = scene.getViewMatrix();

    screen_space_cursor = glm::inverse(persp) * screen_space_cursor;
    screen_space_cursor.z = -1.0f;
    screen_space_cursor.w = 0.0f;
    screen_space_cursor = glm::inverse(view) * screen_space_cursor;

    glm::vec3 ray_world = screen_space_cursor;

    ray_world = glm::normalize(ray_world);


    //std::cout << "\nWorld Space X: " << ray_world.x << "\n";
    //std::cout << "World Space Y: " << ray_world.y << "\n";
    //std::cout << "World Space Z: " << ray_world.z << "\n";

    Raycast ray;
    ray.destination = ray_world;
    ray.origin = scene.eyePosition;

    for (auto i : scene.getEntities())
    {
      if (ray.RayBoxIntersection(i->get<MeshComponent>()->getMeshBounds()) == true)
      {
        Manager.debugRenderer->drawAABB(scene.getEntities()[0]->get<MeshComponent>(), scene, false);
      }
    }
  }*/



  //takes in a texture, window size, and uvs -> draws final outto the whole imgui window
  ImGui::Image((void*)(intptr_t)(Manager.renderer->FinalColorBufferID), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
  ImGui::PopStyleVar();


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
