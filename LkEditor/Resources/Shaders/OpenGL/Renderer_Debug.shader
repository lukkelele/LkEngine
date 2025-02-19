#shader vertex
#version 450 core

layout(location = 0) in vec3   a_Position;
layout(location = 1) in vec3   a_Normal;
layout(location = 2) in vec3   a_Tangent;
layout(location = 3) in vec3   a_Binormal;
layout(location = 4) in vec2   a_TexCoord;
//layout(location = 5) in float  a_TexIndex;
//layout(location = 6) in float  a_TexArray;

out vec3 v_Position;
out vec3 v_Normal;
out vec3 v_Tangent;
out vec3 v_Binormal;
out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ViewProjectionMatrix;

void main()
{
    vec3 WorldPosition = vec3(u_Model * vec4(a_Position, 1.0));
    gl_Position = u_ViewProjectionMatrix * vec4(WorldPosition, 1.0);

    v_Position = vec3(gl_Position);
    v_Normal = a_Normal;
    v_Tangent = a_Tangent;
    v_Binormal = a_Binormal;
    v_TexCoord = a_TexCoord;
}


#shader fragment
#version 450 core
out vec4 FragColor;

in vec3 v_Position; 
in vec3 v_Normal; 
in vec2 v_TexCoord; 

uniform sampler2D u_Texture0;

void main()
{    
    //FragColor = texture(u_Texture0, v_TexCoord);
    FragColor = vec4(v_Position.x, v_Position.y, 0.4, 1.0); 
}
