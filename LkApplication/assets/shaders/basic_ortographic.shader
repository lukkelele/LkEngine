#shader vertex
#version 330 core

layout(location = 0) in vec2 pos;

uniform mat4 u_ViewProj;

void main()
{
    gl_Position = u_ViewProj * vec4(pos, 0.0, 1.0);
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color;
}