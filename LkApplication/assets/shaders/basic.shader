#shader vertex
#version 330 core

// layout(location = 0) in vec4 pos;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 _color;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

out vec2 v_TexCoord; 
out vec4 v_Color;

void main()
{
    gl_Position = u_ViewProj * u_Model * vec4(pos, 1.0);
    // gl_Position = u_ViewProj * vec4(pos, 1.0);
    v_TexCoord = texCoord;
    v_Color = _color;
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;
uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main()
{
    // vec4 texColor = texture(u_Texture, v_TexCoord);
    // color = texColor; 
    color = u_Color;
    // color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}