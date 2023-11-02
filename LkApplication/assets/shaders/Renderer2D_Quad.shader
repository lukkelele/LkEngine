#shader vertex
#version 450 core

layout(location = 0) in vec3     a_Pos;
layout(location = 1) in vec4     a_Color;
layout(location = 2) in vec2     a_TexCoord;
layout(location = 3) in float    a_TexIndex;
layout(location = 4) in float    a_TilingFactor;
layout(location = 5) in int      a_EntityID;

out vec4  v_Color;
out vec2  v_TexCoord;
out float v_TexIndex;
out float v_TilingFactor;
out int   v_EntityID;

uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

void main()
{
    gl_Position = u_ViewProj * vec4(a_Pos, 1.0);

    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

in vec4       v_Color;
in vec2       v_TexCoord;
in float      v_TexIndex;
in float      v_TilingFactor;
flat in int   v_EntityID;

uniform sampler2D u_Texture;
uniform int u_TextureEnabled;

void main()
{
    if (u_TextureEnabled > 0)
    {
        FragColor = texture(u_Texture, v_TexCoord);
    }
    else
    {
        //vec4 solidColor = v_Color; 
        //vec4 darkerColor = solidColor * vec4(0.8, 0.8, 0.8, 1.0)
        //vec4 gradientColor = mix(solidColor, darkerColor, v_TexCoord.y); 
        //FragColor = solidColor * gradientColor;
        // FragColor = gradientColor;

        vec4 baseColor = v_Color; 
        vec4 darkerColor = baseColor * vec4(0.8, 0.8, 0.8, 1.0);
        FragColor = mix(baseColor, darkerColor, v_TexCoord.y);

        //FragColor = v_Color;
    }

}