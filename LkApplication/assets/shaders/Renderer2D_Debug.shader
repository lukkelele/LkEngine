#shader vertex
#version 450 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    v_TexCoords = a_TexCoords;    
    gl_Position = Projection * View * Model * vec4(a_Pos, 1.0);
}


#shader fragment
#version 450 core
out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D Texture1;

void main()
{    
    FragColor = texture(Texture1, v_TexCoords);
}