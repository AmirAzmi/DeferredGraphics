#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 world_position;
in vec3 world_normal_position;
in vec2 texture_coordinates;

uniform vec4 diffuse_color;
uniform float specular_intensity;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = world_position;

    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(world_normal_position);

    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = diffuse_color.rgb;

    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = specular_intensity;
}
