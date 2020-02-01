#pragma once
#include <glm/glm/glm.hpp>
class LightComponent
{

public:
  struct Light
  {

    glm::vec3 ambientColor; //12 bytes 0 % 16
    float constant;         //4 bytes 12 % 4

    glm::vec3 diffuseColor; //12 bytes 16 % 16
    float linear;           //4 bytes 28 % 4

    glm::vec3 specularColor;//12 bytes 32 % 16
    float specularExponent;//4 bytes 44 % 4

    glm::vec3 position;      //padding bytes for next array element
    float quadratic;        //4 bytes 48 % 4

    //attenuation factors
    //-------------------------------
    //Total bytes: 48 bytes however actual total is 64 bytes due to aligning on 16 byte boundary
  };

  Light light;
  LightComponent() :light()
  {
    light.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    light.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    light.specularExponent = 30;
    light.linear = 0.0f;
    light.quadratic = 0.0f;
    light.position = glm::vec3(0.0f,0.0f,5.0f);
  }
};

using LightComponentPtr = LightComponent *;