#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec4 a_Color;
layout (location = 0) out vec4 v_Color;

uniform mat4 u_TransformMatrix;
uniform mat4 u_ViewProj;

void main()
{
	gl_Position = u_ViewProj * vec4(a_Pos, 1.0);
	v_Color = a_Color;
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;
layout (location = 0) in vec4 v_Color;

void main()
{
	color = v_Color;
}