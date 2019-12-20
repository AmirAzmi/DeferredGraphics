#include "RenderingSystem.h"
#include "MeshComponent.h"
#include "EngineTypes.h"

//update the mesh components of the entities
void RenderingSystem::Update(Scene& scene)
{
  //gets all the current mesh components in th scene
  std::vector<MeshComponentPtr> meshes = scene.getMeshes();

  for (auto & meshComponent : meshes)
  {
    //bind the program here and set the material for the shader
    meshComponent->getMaterial()->apply();

    //first bind the vertex array object that you are using
    glBindVertexArray(meshComponent->getMesh()->getVAO());

    //enable position data that will be transferred to the GPU
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, meshComponent->getMesh()->getPosVBO());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //enable normals data that will be transferred to the GPU
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, meshComponent->getMesh()->getNormsVBO());
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);

    //object to world matrix
    glm::mat4 ObjectToWorld = glm::translate(meshComponent->getEntityPtr()->position) * glm::rotate(meshComponent->getEntityPtr()->angle, meshComponent->getEntityPtr()->axisOfRotation) * glm::scale(meshComponent->getEntityPtr()->scale);
    
    //object to world matrix sent to the GPU called "object_to_world"
    GLint umodel_matrix = glGetUniformLocation(meshComponent->getShader()->getProgramID(), "object_to_world");
    glUniformMatrix4fv(umodel_matrix, 1, false, &ObjectToWorld[0][0]);

    //normal matrix sent to the GPU called "normal_matrix"
    GLint unormal_matrix = glGetUniformLocation(meshComponent->getShader()->getProgramID(), "normal_matrix");
    glUniformMatrix4fv(unormal_matrix, 1, false, &(glm::transpose(glm::inverse(ObjectToWorld)))[0][0]);

    //send the variables "perspective_matrix" and "view matrix" onto the GPU
    projectionMatrixID = glGetUniformLocation(meshComponent->getShader()->getProgramID(), "perspective_matrix");
    viewMatrixID = glGetUniformLocation(meshComponent->getShader()->getProgramID(), "view_matrix");

    //get the projection and view matrix from the scene set it as a variables for the GPU
    glUniformMatrix4fv(projectionMatrixID, 1, false, &scene.getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(viewMatrixID, 1, false, &scene.getViewMatrix()[0][0]);


    glDrawElements(meshComponent->getDrawMode(), (GLsizei)meshComponent->getMesh()->getIndexBuffer().size(), GL_UNSIGNED_INT, 0);
  }
}



