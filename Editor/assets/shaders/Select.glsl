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

bool isEdgePixel(vec2 texCoord)
{
    int objectID = int(texture(u_IDImage, texCoord).r);
    for (int i = -EdgeWidth; i <= EdgeWidth; i++)
    {
        for (int j = -EdgeWidth; j <= EdgeWidth; j++)
        {
            vec2 offset = vec2(i, j) / vec2(u_Width, u_Height);
            vec2 neighborCoord = texCoord + offset;
            if (neighborCoord.x >= 0.0 && neighborCoord.x <= 1.0 && neighborCoord.y >= 0.0 && neighborCoord.y <= 1.0)
            {
                int neighborID = int(texture(u_IDImage, neighborCoord).r);
                if (neighborID != objectID && (neighborID == u_SelectedID || objectID == u_SelectedID))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void main()
{
    vec3 color = texture(u_ColorImage, v_TexCoord).rgb;
    if (u_SelectedID != 0)
    {
        // draw the edge of selected object
        if (isEdgePixel(v_TexCoord))
        {
            color = EdgeColor;
        }
    }
    FragColor = vec4(color, 1.0);
}
