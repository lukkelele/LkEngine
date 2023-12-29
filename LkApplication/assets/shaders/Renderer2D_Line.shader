#shader vertex
#version 450 core

layout(location = 0) in vec3  a_Pos;
layout(location = 1) in vec4  a_Color;
layout(location = 2) in int   a_EntityID;

out vec4    v_Color;
out int     v_EntityID;

uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

void main()
{
	gl_Position = u_ViewProj * vec4(a_Pos, 1.0);
	v_Color = a_Color;
	v_EntityID = a_EntityID;
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

in vec4     v_Color;
flat in int v_EntityID;

//uniform sampler2D u_Texture;
//uniform sampler2D u_Textures[32];

void main()
{
	FragColor = v_Color;

    //vec4 tex = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
    //FragColor = tex * v_Color;
}
