#shader vertex
#version 450 core
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
    v_TexCoords = a_TexCoords;
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0); 
}  



#shader fragment
#version 450 core
out vec4 FragColor;

in vec2 v_TexCoords;

uniform sampler2D ScreenTexture;

void main()
{
    vec3 col = texture(ScreenTexture, v_TexCoords).rgb;
    FragColor = vec4(col, 1.0);
} 

