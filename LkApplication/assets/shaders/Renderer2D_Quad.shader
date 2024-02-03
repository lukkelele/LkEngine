#shader vertex
#version 450 core

layout(location = 0) in vec3     a_Pos;
layout(location = 1) in vec4     a_Color;
layout(location = 2) in vec2     a_TexCoord;
layout(location = 3) in float    a_TexIndex;
layout(location = 4) in float    a_TilingFactor;
layout(location = 5) in int      a_EntityID;

out vec4     v_Color;
out vec2     v_TexCoord;
out float    v_TexIndex;
out float    v_TilingFactor;
out int      v_EntityID;

uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

void main()
{
    gl_Position = u_ViewProj * vec4(a_Pos, 1.0);

    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;
    v_TexIndex = a_TexIndex;
}


#shader fragment
#version 450 core
//#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 FragColor;

in vec4       v_Color;
in vec2       v_TexCoord;
in float      v_TexIndex;
in float      v_TilingFactor;
flat in int   v_EntityID;

uniform sampler2DArray u_TextureArray1;
uniform sampler2DArray u_TextureArray2;
uniform sampler2DArray u_TextureArray3;
uniform sampler2DArray u_TextureArray4;
uniform sampler2DArray u_TextureArray5;


void main()
{
    //vec4 tex = vec4(0.0);
    //switch(int(v_TexIndex)) 
    //{
    //    case 0: 
    //        tex = texture(u_TextureArray1, vec3(v_TexCoord, v_TexIndex));
    //        break;
    //    case 3: 
    //        tex = texture(u_TextureArray3, vec3(v_TexCoord, v_TexIndex));
    //        break;
    //    default:
    //        break;
    //}
    vec4 tex = texture(u_TextureArray1, vec3(v_TexCoord, v_TexIndex));
    FragColor = tex * v_Color;
}

