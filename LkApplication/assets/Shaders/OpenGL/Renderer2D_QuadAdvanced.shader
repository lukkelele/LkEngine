#shader vertex
#version 450 core

layout(location = 0) in vec3  a_Pos;
layout(location = 1) in vec2  a_TexCoord;
layout(location = 2) in vec4  a_Color;
layout(location = 3) in float a_EntityID;

out vec4  v_Color;
out float v_EntityID;
out vec2  v_TexCoord;

uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

void main()
{
    gl_Position = u_ViewProj * vec4(a_Pos, 1.0);

    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_EntityID = a_EntityID;
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

in vec4  v_Color;
in vec2  v_TexCoord;
in float v_EntityID;

uniform sampler2D u_Texture;

uniform vec4 u_Color;

void main()
{
    vec4 color;
    if (true)
    {
        color = v_Color;
    }
    else
    {
        //color = texture(u_Texture, v_TexCoord);
        color = v_Color;
    }
    FragColor = color;
}