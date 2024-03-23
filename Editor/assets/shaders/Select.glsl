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

uniform sampler2D u_ColorImage;
uniform sampler2D u_IDImage;
uniform int u_SelectedID;
uniform int u_Width;
uniform int u_Height;

const int EdgeWidth = 1;
const vec3 EdgeColor = vec3(1.0, 1.0, 0.0);
void main()
{
    vec3 color = texture(u_ColorImage, v_TexCoord).rgb;
    //if (u_SelectedID != 0)
    //{
    //    vec2 u_Step = vec2(1.0 / float(u_Width), 1.0 / float(u_Height));
    //    vec2 uv = v_TexCoord;
    //    vec2 uv1 = uv + vec2(EdgeWidth * u_Step.x, 0.0);
    //    vec2 uv2 = uv + vec2(-EdgeWidth * u_Step.x, 0.0);
    //    vec2 uv3 = uv + vec2(0.0, EdgeWidth * u_Step.y);
    //    vec2 uv4 = uv + vec2(0.0, -EdgeWidth * u_Step.y);
    //    int id1 = int(texture(u_IDImage, uv1).r);
    //    int id2 = int(texture(u_IDImage, uv2).r);
    //    int id3 = int(texture(u_IDImage, uv3).r);
    //    int id4 = int(texture(u_IDImage, uv4).r);
    //    if (id1 == u_SelectedID || id2 == u_SelectedID || id3 == u_SelectedID || id4 == u_SelectedID)
    //    {
    //        color = EdgeColor;
    //    }
    //    
    //}
    FragColor = vec4(color, 1.0);
}
