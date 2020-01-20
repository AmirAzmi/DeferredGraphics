#version 430 core

in vec2 texture_coordinates;

out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 view_position;

struct Light
{
  vec3 ambientColor; //12 bytes 0 % 16
  float constant;         //4 bytes 12 % 4

  vec3 diffuseColor; //12 bytes 16 % 16
  float linear;           //4 bytes 28 % 4

  vec3 specularColor;//12 bytes 32 % 16
  float specularIntensity;//4 bytes 44 % 4

  vec3 position;      //padding bytes for next array element
  float quadratic;        //4 bytes 48 % 4

  //attenuation factors
  //-------------------------------
  //Total bytes: 48 bytes however actual total is 64 bytes due to aligning on 16 byte boundary
};

layout (std430, binding = 0) buffer shader_data
{
  Light lights[16];
  int numberOfLights;
};

void main()
{
 //retrieve data from gbuffer
 vec3 world_position = texture(gPosition, texture_coordinates).rgb;
 vec3 normal_world_position = texture(gNormal, texture_coordinates).rgb;
 vec3 diffuse_color = texture(gAlbedoSpec, texture_coordinates).rgb;
 float specular_intensity = texture(gAlbedoSpec, texture_coordinates).a;
  
  //
  vec3 lighting = diffuse_color;

  //for all lights
  for(int i = 0; i <  numberOfLights; ++i)
  {
     Light light = lights[i];
     
     float dist = distance(light.position, world_position); //distance from light source to fragment

     // diffuse
     vec3 light_direction = normalize(light.position - world_position);
     vec3 diffuse = max(dot(normal_world_position, light_direction), 0.0) * diffuse_color * light.ambientColor;

     //specular
     vec3 reflection_vector = normalize(vec3(2 * dot(normal_world_position,light_direction) * normal_world_position - light_direction)); //reflection vector  
     float spec = pow(max(dot(normal_world_position, reflection_vector), 0.0), specular_intensity);
     vec3 specular = light.ambientColor * spec * specular_intensity;

     // attenuation
     float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);
     diffuse *= attenuation;
     specular *= attenuation;
     lighting += diffuse_color + specular;

  }

  color = vec4(lighting, 1.0);
}
