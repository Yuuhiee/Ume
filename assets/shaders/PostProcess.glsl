#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_Texcoord;
    gl_Position = vec4(a_Position.xy, 0.0, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform int u_Gamma;
uniform int u_HDR;
uniform float u_Exposure;
    
void main()
{
    vec3 color = texture(u_Texture, v_TexCoord).rgb;
    
    if (bool(u_HDR))
        color = vec3(1.0) - exp(-color * u_Exposure); 
        
    if (bool(u_Gamma))
        color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, 1.0);
}
