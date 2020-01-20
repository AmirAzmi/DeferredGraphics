#include <iostream>
#include <algorithm>
#include "RenderingSystem.h"
#include "MeshComponent.h"
#include "EngineTypes.h"
#include "Entity.h"

RenderingSystem::RenderingSystem(int windowWidth, int windowHeight) :projectionMatrixID(-1), viewMatrixID(-1)
{
  //set the deffered lighting shader that should be used
  defferedLightingShaderID = std::make_shared<Shader>("defferedLightingPass.vert", "defferedLightingPass.frag", true);
  forwardLightingShaderID = std::make_shared <Shader>("forwardLightingPass.vert", "forwardLightingPass.frag", false);

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

  //generate the framebuffer to draw to
  glGenFramebuffers(1, &gBufferFBOID);
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBOID);

  //generate the textures for the position data
  glGenTextures(1, &gPositionID);
  glBindTexture(GL_TEXTURE_2D, gPositionID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionID, 0);

  //generate the textures for the normals
  glGenTextures(1, &gNormalID);
  glBindTexture(GL_TEXTURE_2D, gNormalID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalID, 0);

  //generate the textures for the specular + color aka emissive aka albedo
  glGenTextures(1, &gColorSpecID);
  glBindTexture(GL_TEXTURE_2D, gColorSpecID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpecID, 0);

  //tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(3, attachments);

  //create and attach depth buffer (renderbuffer)
  glGenRenderbuffers(1, &rboDepthID);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthID);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthID);

  //check if framebuffer is complete and unbind the framebuffer
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // shader configuration
  // for deffered shader
  // --------------------
  defferedLightingShaderID->UseShader();
  defferedLightingShaderID->setInt("gPosition", 0);
  defferedLightingShaderID->setInt("gNormal", 1);
  defferedLightingShaderID->setInt("gAlbedoSpec", 2);
}

//update the mesh components of the entities
void RenderingSystem::Update(Scene& scene, int windowWidth, int windowHeight)
{
  //gets all the current mesh components in th scene
  std::vector<MeshComponentPtr> meshes = scene.getMeshes();

  //gets all the current mesh components in th scene
  std::vector<LightComponentPtr> lights = scene.getLights();

  //Geometry Pass
  //------------------------------------------------------------
  //first it sorts the list where the values from getDeffered are put into buckets
  auto iterator_to_forward_list = std::partition(meshes.begin(), meshes.end(), [](MeshComponentPtr mesh) {return mesh->getShader()->getDeffered(); });

  //bind the g framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBOID);

  //clear color buffer bit and depth buffer info from the default framebuffer
  glClearColor(0.5f,0.3f, 0.2f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //ssbo loop thorugh lights for light mapping for deffered lights
  int index = 0;
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

  //bind the ssbo for each shader and memcpy the light data into the lights
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID[1]);
  GLvoid* q = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
  shader_data.numberOfLights = index;
  memcpy(q, &shader_data, sizeof(shader_data));
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

  //draw deffered objectrs first
  std::for_each(meshes.begin(), iterator_to_forward_list, [&scene, this](MeshComponentPtr mesh) {Draw(mesh, scene, true); });

  //unbind the g framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //lighting Pass
  //------------------------------------------------------------
  glClearColor(0.5f, 0.3f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  defferedLightingShaderID->UseShader();

  //set the active textures to be displayed on the quad
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPositionID);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormalID);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gColorSpecID);

  defferedLightingShaderID->setVec3("view_position", scene.getEyePosition());

  //render quads
  DrawQuad();

  //copy content of geometry's depth buffer to default framebuffer's depth buffer
  //----------------------------------------------------------------------------------
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBOID);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
  //blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
  //the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
  //depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
  glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //draw forward objects next
  std::for_each(iterator_to_forward_list, meshes.end(), [&scene, this](MeshComponentPtr mesh) {Draw(mesh, scene, false); });
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
    glDrawElements(mesh->getDrawMode(), (GLsizei)mesh->getMesh()->getIndexBuffer().size(), GL_UNSIGNED_INT, 0);
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
    std::cout << "";

  }

  //draw the quad data and unbind the VAO for other things to be drawn
  glBindVertexArray(quadVAOID);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}



