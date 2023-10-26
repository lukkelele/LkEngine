#shader vertex
#version 450 core

layout(location = 0) in vec2 a_Pos;
uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;


void main()
{
    gl_Position = u_ViewProj * u_TransformMatrix * vec4(a_Pos, 0.0, 1.0);
}



#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color;
}