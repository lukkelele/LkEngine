#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

out vec2 v_TexCoord;

void main()
{
    gl_Position = u_ViewProj * vec4(a_Pos, 1.0);
    v_TexCoord = a_TexCoord;
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

uniform vec4 u_Color;
uniform bool u_TexturesEnabled;

in vec2 v_TexCoord;
uniform sampler2D u_Texture;

void main()
{
    vec4 color;

    if (u_TexturesEnabled)
    {
        //vec4 texColor = texture(u_Texture, v_TexCoord);
        color = texture(u_Texture, v_TexCoord);
    }
    else
    {
        color = u_Color;
    }

    FragColor = color;
}