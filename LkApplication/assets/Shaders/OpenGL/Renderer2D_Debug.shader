#shader vertex
#version 450 core

layout(location = 0) in vec3   a_Position;
layout(location = 1) in vec4   a_Color;
layout(location = 2) in vec2   a_Texcoord;
layout(location = 3) in float  a_TexIndex;
layout(location = 4) in float  a_TexArray;
layout(location = 5) in float  a_TilingFactor;

out vec2 v_Texcoord;
out float v_TexIndex;
out float v_TexArray;

uniform mat4 u_Model;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewProjectionMatrix;


void main()
{
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Model * vec4(a_Position, 1.0);

    v_Texcoord = a_Texcoord;    
    v_TexIndex = a_TexIndex;
    v_TexArray = a_TexArray;
}


#shader fragment
#version 450 core
out vec4 FragColor;

in vec2 v_Texcoord;

uniform sampler2D Texture1;


void main()
{    
    FragColor = texture(Texture1, v_Texcoord);
    FragColor = vec4(v_Texcoord.x, v_Texcoord.y, 1, 1);
}