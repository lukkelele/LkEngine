#shader vertex
#version 450 core

layout(location = 0) in vec3     a_Position;
layout(location = 1) in vec4     a_Color;
layout(location = 2) in vec2     a_Texcoord;
layout(location = 3) in float    a_TexIndex;
layout(location = 4) in float    a_TexArray;
layout(location = 5) in float    a_TilingFactor;


out vec4     v_Color;
out vec2     v_Texcoord;
out float    v_TexIndex;
out float    v_TexArray;
out float    v_TilingFactor;


layout(std140, binding = 0) uniform UB_Camera
{
    mat4 u_ViewProjection;
};


void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);

    v_Color = a_Color;
    v_Texcoord = a_Texcoord;
    v_TexIndex = a_TexIndex;
    v_TexArray = a_TexArray;
    v_TilingFactor = a_TilingFactor;
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

in vec4    v_Color;
in vec2    v_Texcoord;
in float   v_TexIndex;
in float   v_TexArray;
in float   v_TilingFactor;

uniform sampler2DArray u_TextureArray1;
uniform sampler2DArray u_TextureArray2;
uniform sampler2DArray u_TextureArray3;


void main()
{
    vec4 tex = vec4(0.0);
    switch(int(v_TexArray)) 
    {
        case 0: tex = texture(u_TextureArray1, vec3(v_Texcoord, v_TexIndex)); break;
        case 1: tex = texture(u_TextureArray2, vec3(v_Texcoord, v_TexIndex)); break;
        case 2: tex = texture(u_TextureArray3, vec3(v_Texcoord, v_TexIndex)); break;
    }
    //FragColor = vec4(1, 1 * v_TexArray, 1 * v_TexArray, 1.0);
    FragColor = tex * v_Color;
}

