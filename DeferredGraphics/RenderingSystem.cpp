/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: RenderingSystem.cpp
Purpose: First system of ECS to draw objects
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#include <iostream>
#include <algorithm>
#include "RenderingSystem.h"
#include "MeshComponent.h"
#include "EngineTypes.h"
#include "Entity.h"

RenderingSystem::RenderingSystem(const int windowWidth, const int windowHeight) :projectionMatrixID(-1), viewMatrixID(-1)
{
  //set the deffered lighting shader that should be used
  defferedLightingShaderID = std::make_shared<Shader>("Resources/Shaders/defferedLightingPass.vert", "Resources/Shaders/defferedLightingPass.frag", true);
  forwardLightingShaderID = std::make_shared<Shader>("Resources/Shaders/forwardLightingPass.vert", "Resources/Shaders/forwardLightingPass.frag", false);
  gBufferShaderID = std::make_shared<Shader>("Resources/Shaders/gBuffer.vert", "Resources/Shaders/gBuffer.frag", true);
  colorAndBrightShaderID = std::make_shared<Shader>("Resources/Shaders/ColorAndBright.vert", "Resources/Shaders/ColorAndBright.frag", true);
  bloomFinalID = std::make_shared<Shader>("Resources/Shaders/FinalBloom.vert", "Resources/Shaders/FinalBloom.frag", true);
  splitScreenShaderID = std::make_shared<Shader>("Resources/Shaders/gBufferViewer.vert", "Resources/Shaders/gBufferViewer.frag", false);
  finalColorID = std::make_shared<Shader>("Resources/Shaders/FinalColorOut.vert", "Resources/Shaders/FinalColorOut.frag", false);

  //generate the first ssbo
  glGenBuffers(2, &ssboID[0]);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID[0]);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);

  //generate the second ssbo
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID[1]);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  //initialize the ssbo for the deffered lighting pass
  blockIndex[0] = glGetProgramResourceIndex(defferedLightingShaderID->getProgramID(), GL_SHADER_STORAGE_BLOCK, "shader_data");
  glShaderStorageBlockBinding(defferedLightingShaderID->getProgramID(), blockIndex[0], ssboBindingPointIndex[0]);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboBindingPointIndex[0], ssboID[0]);

  //initialize the ssbo for the forward lighting pass
  blockIndex[1] = glGetProgramResourceIndex(forwardLightingShaderID->getProgramID(), GL_SHADER_STORAGE_BLOCK, "shader_data");
  glShaderStorageBlockBinding(forwardLightingShaderID->getProgramID(), blockIndex[1], ssboBindingPointIndex[1]);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboBindingPointIndex[1], ssboID[1]);

  /********************************************************************************************/
  //BIG NOTE: Layout Location in Shader only matters for what framebuffer is binded to
  //generate the framebuffer to draw to
  glGenFramebuffers(1, &gBufferFBOID);
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBOID);

  //generate the textures for the position data
  glGenTextures(1, &gPositionID);
  glBindTexture(GL_TEXTURE_2D, gPositionID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionID, 0);

  //generate the textures for the normals
  glGenTextures(1, &gNormalID);
  glBindTexture(GL_TEXTURE_2D, gNormalID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalID, 0);

  //generate the textures for the specular + color aka emissive aka albedo
  glGenTextures(1, &gColorSpecID);
  glBindTexture(GL_TEXTURE_2D, gColorSpecID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpecID, 0);

  //tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

  //draws all 3 buffers and the hdr buffer
  glDrawBuffers(3, attachments);

  //create and attach depth buffer (renderbuffer)
  glGenRenderbuffers(1, &rboDepthID);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthID);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthID);

  //check if framebuffer is complete and unbind the framebuffer
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;

  /********************************************************************************************/
  //generate the framebuffer Lighting
  glGenFramebuffers(1, &LightingPassFBOID);
  glBindFramebuffer(GL_FRAMEBUFFER, LightingPassFBOID);

  //generate the texture for BloomBuffer/ Bright Color values over 190
  glGenTextures(1, &ColorBufferID);
  glBindTexture(GL_TEXTURE_2D, ColorBufferID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBufferID, 0);

  //generate the texture for BloomBuffer/ Bright Color values over 190
  glGenTextures(1, &BrightBufferID);
  glBindTexture(GL_TEXTURE_2D, BrightBufferID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, BrightBufferID, 0);

  //tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachment[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

  //draws all 3 buffers and the hdr buffer
  glDrawBuffers(2, attachment);

  glGenRenderbuffers(1, &rboDepthID3);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthID3);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthID3);

  //check if above frame buffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;

  /********************************************************************************************/
  //Final Color FBO
  glGenFramebuffers(1, &FinalFBOID);
  glBindFramebuffer(GL_FRAMEBUFFER, FinalFBOID);

  //generate the texture for the final color output
  glGenTextures(1, &FinalColorBufferID);
  glBindTexture(GL_TEXTURE_2D, FinalColorBufferID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FinalColorBufferID, 0);

  //tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachment_final[1] = { GL_COLOR_ATTACHMENT0 };

  //draws the final texture
  glDrawBuffers(1, attachment_final);

  //create and attach depth buffer (renderbuffer)
  glGenRenderbuffers(1, &rboDepthID2);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthID2);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthID2);

  //check if above frame buffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;

  /********************************************************************************************/
  //generate the framebuffers needed for gaussian blurr horizontally and vertically
  glGenFramebuffers(2, PingPongFBO);
  glGenTextures(2, PingPongColorBuffer);
  for (unsigned int i = 0; i < 2; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[i]);
    glBindTexture(GL_TEXTURE_2D, PingPongColorBuffer[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PingPongColorBuffer[i], 0);
    // also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;
  }

  /********************************************************************************************/
  // shader configuration
  // for deffered shader
  // for bloom shader
  // --------------------
  defferedLightingShaderID->UseShader();
  defferedLightingShaderID->setInt("gPosition", 0);
  defferedLightingShaderID->setInt("gNormal", 1);
  defferedLightingShaderID->setInt("gAlbedoSpec", 2);

  colorAndBrightShaderID->UseShader();
  colorAndBrightShaderID->setInt("ucolor", 0);
  colorAndBrightShaderID->setInt("BrightColor", 1);

  bloomFinalID->UseShader();
  bloomFinalID->setInt("ucolor", 0);
  bloomFinalID->setInt("BloomColor", 1);
}

RenderingSystem::~RenderingSystem()
{
  //delete shader attachments
  glDeleteBuffers(0, &gColorSpecID);
  glDeleteBuffers(0, &gNormalID);
  glDeleteBuffers(0, &gPositionID);
  glDeleteBuffers(0, &BrightBufferID);

  //delete framebuffer container of the attachements
  glDeleteFramebuffers(0, &gBufferFBOID);
}

//update the mesh components of the entities
void RenderingSystem::Update(Scene& scene, const int windowWidth, const int windowHeight)
{
  auto start = std::chrono::high_resolution_clock::now();

  glDepthFunc(GL_LEQUAL);
  //gets all the current mesh components in th scene
  std::vector<MeshComponentPtr>& meshes = scene.getMeshes();

  //gets all the current mesh components in th scene
  std::vector<LightComponentPtr>& lights = scene.getLights();

  //Geometry Pass
  //------------------------------------------------------------
  //first it sorts the list where the values from getDeffered are put into buckets
  auto iterator_to_forward_list = std::partition(meshes.begin(), meshes.end(), [](MeshComponentPtr mesh) {return mesh->getShader()->getDeffered(); });

  //bind the g framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBOID);


  //clear color buffer bit and depth buffer info from the default framebuffer
  glClearColor(0.5f, 0.3f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //use the shader that needs the information of the position, normals, and color/specular
  gBufferShaderID->UseShader();

  //draw deffered objects first well in this case draw only the geometry of the deffered objects
  //first ans save the lighting pass for the when we bind the lighting FBO
  //write objects to the currently binded frame buffer and fills in the data of position, normals, and color/specular
  //which can now be used for the next shader and draw calls
  std::for_each(meshes.begin(), iterator_to_forward_list, [&scene, this](MeshComponentPtr mesh) {Draw(mesh, scene, true); });


  //set the active textures to be displayed on the quad with the geometry
  //so what is happening here is we are binding the textures we stored from the g buffer
  //now this data can be used as uniform samplers for the deffered lighting shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPositionID);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormalID);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gColorSpecID);

  //lighting Pass in the default framebuffer, everything is drawn to the quad plane 
  //------------------------------------------------------------
  //bind the lighting pass framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, LightingPassFBOID);

  //call the clear color stuff per frame buffer
  glClearColor(0.5f, 0.3f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //use the correct shader program in this case we do deffered drawing of items first
  //since we binded the textures from above, they are now used as uniform samplers for the next
  //draw call
  defferedLightingShaderID->UseShader();
  defferedLightingShaderID->setVec3("view_position", scene.getEyePosition());

  //lighting information for deffered objects
  int index = 0; //index of the light in the shader sata struct
  {
    //ssbo loop thorugh lights for light mapping for deffered lights
    for (const auto& light : lights)
    {
      shader_data.lights[index] = light->light;
      ++index;
    }

    //bind the ssbo for each shader and memcpy the light data into the lights
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID[0]);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    shader_data.numberOfLights = index;
    memcpy(p, &shader_data, sizeof(shader_data));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  }

  //writing data to the currently bounded framebuffer, more importantly
  //this draw call fills in the information of the color and bright buffer to be
  //used for the next bounded shader and draw calls that are effected by that
  //shader
  DrawQuad();
  glDepthMask(GL_TRUE);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBOID); //gets the data stored in the gBuffer framebuffer
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, LightingPassFBOID); // write to default framebuffer with information from the the gBuffer
  glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  //use the lighting shader and set its material also this is where we draw forward rendered objects
  //in the default framebuffer
  //------------------------------------------------------------
  forwardLightingShaderID->UseShader();

  //lighting information for the forward objects
  {
    //bind the ssbo for each shader and memcpy the light data into the lights
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID[1]);
    GLvoid* q = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    shader_data.numberOfLights = index;
    memcpy(q, &shader_data, sizeof(shader_data));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  }

  forwardLightingShaderID->setVec3("view_position", scene.getEyePosition());

  //draw forward objects next
  std::for_each(iterator_to_forward_list, meshes.end(), [&scene, this](MeshComponentPtr mesh) {Draw(mesh, scene, false); });

  //read the data that was written from drawing the quad and use it as uniforms for the shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ColorBufferID);

  //this binds the bright color values that need to be bloomed
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, BrightBufferID);

  //since draw call was made, the two outs from the deffered shader were the color buffer 
  //and bright buffer, thus allowing us to use those as uniform samplers in the next bounded
  //shader which is the color and bright shader
  colorAndBrightShaderID->UseShader();

  bool horizontal = true, first_iteration = true;
  int amount = 10;
  for (unsigned int i = 0; i < amount; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, PingPongFBO[horizontal]);

    glBindTexture(GL_TEXTURE_2D, first_iteration ? BrightBufferID : PingPongColorBuffer[!horizontal]);
    colorAndBrightShaderID->setInt("horizontal", horizontal);

    //writes information into the bright buffer shader
    DrawQuad();

    //this allows for the swapping of the color buffers
    horizontal = !horizontal;

    if (first_iteration)
    {
      first_iteration = false;
    }
  }

  //The last texture now contains the true colors of the objects, to display the colors of the object on
  //screen, You must draw within the default framebuffer by first
  //setting the default framebuffer and then drawing the quad with the textures
  glBindFramebuffer(GL_FRAMEBUFFER, FinalFBOID);

  //resetting the clear color, i might not need to do this here but its proper ettiquete to
  //clear color and its flags at the start of the new binded framebuffer
  glClearColor(0.5f, 0.3f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  bloomFinalID->UseShader();
  bloomFinalID->setInt("bloom", bloom);
  bloomFinalID->setFloat("exposure", exposure);
  bloomFinalID->setFloat("gamma_correction", gamma);
  bloomFinalID->setFloat("exposure_tone_mapping", exposure_tone_mapping);
  bloomFinalID->setFloat("uncharted_tone_mapping", uncharted_tone_mapping);

  //the draw call above filled in the data for the color buffer and the final ping pong buffer
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ColorBufferID);

  //this binds the bloomed color values
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, PingPongColorBuffer[!horizontal]);

  //draw final output
  glDepthMask(GL_FALSE);

  //if split screen is true, top right should contain total image value using draw quads else we should draw the
  //quad to the whole screen
  if (splitScreen == true)
  {
    glViewport(windowWidth / 2, windowHeight / 2, windowWidth / 2, windowHeight / 2);
    DrawQuad();
  }
  else if (brightBuffer == true)
  {
    glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight);
    DrawQuad();
  }
  else
  {
    //draw the quad, this draws every texture that was binded within the the last framebuffer
    //that was binded, in this case 3 textures were binded and the last fbo binded was the lighting
    //pass fbo
    glViewport(0, 0, windowWidth, windowHeight);

    //this draw quad and the ones above it contain the collective final image of the bloomed and color textures combined
    DrawQuad();
  }

  glDepthMask(GL_TRUE);

  if (depthCopyToggle == true)
  {
    //copy content of geometry's depth buffer to default framebuffer's depth buffer
    //----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBOID); //gets the data stored in the gBuffer framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FinalFBOID); // write to default framebuffer with information from the the gBuffer
    //blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    //the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
    //depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    if (splitScreen == false && brightBuffer == false)
    {
      glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }   
    else if (splitScreen == true)
    {
      glBlitFramebuffer(0, 0, windowWidth, windowHeight, windowWidth / 2, windowHeight / 2, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
    else if (brightBuffer == true)
    {
      glBlitFramebuffer(0, 0, windowWidth, windowHeight, windowWidth / 2, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
  }

  //need to draw textures after the depth toggle 
  //for correct rendering of the split screen
  if (splitScreen == true)
  {
    //bottom left
    DrawTextures(gPositionID, 0, 0, windowWidth / 2, windowHeight / 2);

    //bottom right
    DrawTextures(gNormalID, windowWidth / 2, 0, windowWidth / 2, windowHeight / 2);

    //top left
    DrawTextures(gColorSpecID, 0, windowHeight / 2, windowWidth / 2, windowHeight / 2);
  }

  if (brightBuffer == true)
  {
    DrawTextures(BrightBufferID, 0, 0, windowWidth / 2, windowHeight);
  }

  auto end = std::chrono::high_resolution_clock::now();
  rendering_sytem_elapsed_time = end - start;
}

void RenderingSystem::Draw(MeshComponentPtr mesh, Scene& scene, bool isDeffered)
{
  if (mesh->getShader()->getDeffered() == isDeffered)
  {
    //bind the program here and set the material for the shader    
    mesh->getMaterial()->apply();

    //first bind the vertex array object that you are using
    glBindVertexArray(mesh->getMesh()->getVAO());

    //enable position data that will be transferred to the GPU
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->getMesh()->getPosVBO());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //enable normals data that will be transferred to the GPU
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->getMesh()->getNormsVBO());
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);

    //enable UV data that will be transferred to the GPU

    /*glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->getMesh()->getUVBO());
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    */

    //bind the index buffer that will be transferred to the GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getMesh()->getIndexVBO());

    //object to world matrix
    glm::mat4 ObjectToWorld = glm::translate(mesh->getEntityPtr()->position) * glm::rotate(mesh->getEntityPtr()->angle, mesh->getEntityPtr()->axisOfRotation) * glm::scale(mesh->getEntityPtr()->scale);

    //object to world matrix sent to the GPU called "object_to_world"
    GLint umodel_matrix = glGetUniformLocation(mesh->getShader()->getProgramID(), "object_to_world");
    glUniformMatrix4fv(umodel_matrix, 1, false, &ObjectToWorld[0][0]);

    //normal matrix sent to the GPU called "normal_matrix"
    GLint unormal_matrix = glGetUniformLocation(mesh->getShader()->getProgramID(), "normal_matrix");
    glUniformMatrix4fv(unormal_matrix, 1, false, &(glm::transpose(glm::inverse(ObjectToWorld)))[0][0]);

    //send the variables "perspective_matrix" and "view matrix" onto the GPU
    projectionMatrixID = glGetUniformLocation(mesh->getShader()->getProgramID(), "perspective_matrix");
    viewMatrixID = glGetUniformLocation(mesh->getShader()->getProgramID(), "view_matrix");

    //get the projection and view matrix from the scene set it as a variables for the GPU
    glUniformMatrix4fv(projectionMatrixID, 1, false, &scene.getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(viewMatrixID, 1, false, &scene.getViewMatrix()[0][0]);

    //draw the objects with the mesh components
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh->getMesh()->getIndices().size(), GL_UNSIGNED_INT, 0);
  }
}

void RenderingSystem::DrawQuad()
{
  //this means that the quadVAO is null and I want to create a quad so give me a real ID
  if (quadVAOID == 0)
  {
    float quadVertices[] =
    {
      // positions        
      -1.0f,  1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
    };

    float quadUVs[] =
    {
      // texture Coords
      0.0f, 1.0f,
      0.0f, 0.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
    };

    //generate the vao for the quad
    glGenVertexArrays(1, &quadVAOID);

    //generate the vertex and UV buffers for the quad
    glGenBuffers(1, &quadVBOID);
    glGenBuffers(1, &quadUVID);

    //bind the VAO for transferring of data to the GPU
    glBindVertexArray(quadVAOID);

    //enable position data that will be transferred to the GPU from the quad vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //enable uv data that will be transferred to the GPU
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, quadUVID);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadUVs), &quadUVs, GL_STATIC_DRAW);
  }

  //draw the quad data and unbind the VAO for other things to be drawn
  glBindVertexArray(quadVAOID);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void RenderingSystem::DrawTextures(GLuint textureID, const unsigned posX, const unsigned posY, const unsigned windowWidth, const unsigned windowHeight)
{
  
  glDisable(GL_DEPTH_TEST);

  //set the viewport
  glViewport(posX, posY, windowWidth, windowHeight);

  //activate the txture
  glActiveTexture(GL_TEXTURE0);

  //bind the texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  //set the shader in use
  splitScreenShaderID->UseShader();

  //set uniform for access in the shader
  GLuint textureLocation = glGetUniformLocation(splitScreenShaderID->getProgramID(), "texture");
  glUniform1i(textureLocation, 0);

  if (splitScreenVAOID == 0)
  {
    float Vertices[] =
    {
      // positions        
      -1.0f,  1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
    };

    float UVs[] =
    {
      // texture Coords
      0.0f, 1.0f,
      0.0f, 0.0f,
      1.0f, 1.0f,
      1.0f, 0.0f,
    };

    //generate the VAO
    glGenVertexArrays(1, &splitScreenVAOID);

    //bind the buffer data
    glGenBuffers(1, &splitscreenUVID);
    glGenBuffers(1, &splitScreenVBOID);

    //bind the VAO for transferring of data to the GPU
    glBindVertexArray(splitScreenVAOID);

    //enable the position data
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, splitScreenVBOID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    //enable the position data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, splitscreenUVID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);
  }

  //bind the VAO for transferring of data to the GPU
  glBindVertexArray(splitScreenVAOID);

  //draw the textures
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  //after drawing the VAO in use
  glBindVertexArray(0);

  //unbind the texture once its used
  splitScreenShaderID->UnBindShader();
  glDisable(GL_DEPTH_TEST);
}



