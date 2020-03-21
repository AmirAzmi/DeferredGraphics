#version 330 core

out vec4 color;

in vec2 texture_coordinates;

uniform sampler2D ucolor;
uniform sampler2D BrightColor;
uniform float exposure;
uniform bool bloom;
uniform bool gamma_correction;
uniform bool exposure_tone_mapping;
uniform bool uncharted_tone_mapping;

//uncharted tone mapping function
vec3 Uncharted2Tonemap(vec3 x)
{
  float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

  return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{             

  vec3 hdrColor = texture(ucolor, texture_coordinates).rgb;      
  vec3 bloomColor = texture(BrightColor, texture_coordinates).rgb;

  const float gamma = 2.2f;
  vec3 result = hdrColor;

  if(bloom)
  {
    hdrColor += bloomColor; // additive blending
  }

  //doing the tone mapping and gamma correction after lighting calculations is interesting and correct
  //exposure tone mapping

  if(exposure_tone_mapping)
  {
    result = vec3(1.0) - exp(-hdrColor * exposure);
  }

  if(uncharted_tone_mapping)
  {
     result = Uncharted2Tonemap(hdrColor);
  }

  if(gamma_correction)
  {
    result = pow(result, vec3(1.0/gamma));
  }

    color = vec4(result, 1.0);
}