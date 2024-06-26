#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = a_Position;
    vec4 pos = projection * view * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}  


#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
