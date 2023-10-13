#shader vertex
#version 450 core

layout(location = 0) in vec2 pos;
 
void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
}


#shader fragment
#version 450 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.5, 1.0);
}