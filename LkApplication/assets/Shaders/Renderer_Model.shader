#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main()
{
    v_TexCoords = a_TexCoords;    
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Pos, 1.0);
}


#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform sampler2D u_Normal1;
uniform sampler2D u_Height1;

void main()
{    
    FragColor = texture(u_Diffuse1, v_TexCoords);
}