#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
uniform mat4 u_TransformMatrix;

void main()
{
    gl_Position = u_TransformMatrix * vec4(position, 0.0, 1.0);
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color;
}
