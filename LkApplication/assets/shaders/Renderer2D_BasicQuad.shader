#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

//out vec4 o_DebugColor;


void main()
{
    //gl_Position = u_ViewProj * u_TransformMatrix * vec4(a_Pos, 1.0);
    gl_Position = u_ViewProj * vec4(a_Pos, 1.0);
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
//in vec4 v_DebugColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color;
    //FragColor = v_DebugColor;
}