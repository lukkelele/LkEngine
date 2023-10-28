#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec4 a_Color;
layout(location = 3) in float a_EntityID;

out vec4 v_Color;
out float v_EntityID;

uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

void main()
{
    gl_Position = u_ViewProj * vec4(a_Pos, 1.0);
    v_Color = a_Color;
    v_EntityID = a_EntityID;
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in float v_EntityID;

uniform vec4 u_Color;

void main()
{
    FragColor = v_Color;
    //FragColor = u_Color;
}