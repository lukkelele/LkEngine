#shader vertex
#version 450 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

out vec3 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewProjection;

void main()
{
    vec3 WorldPos = vec3(u_Model * vec4(a_Position, 1.0));
    gl_Position = u_ViewProjection * vec4(WorldPos, 1.0);
    //gl_Position = u_ViewProjection * vec4(a_Position, 1.0);

    v_TexCoord = a_Position;
    //v_TexCoord = vec3(a_Position.xy, -a_Position.z);
}


#shader fragment
#version 450 core
out vec4 FragColor;

in vec3 v_TexCoord;

uniform vec3 u_CameraPos;
uniform samplerCube u_Skybox;

void main()
{    
    //vec3 I = normalize(v_Pos, u_CameraPos);
    //vec3 R = reflect(vec3(1, 1, 1), normalize(v_Normal));
    //vec3 R = vec3(1, 1, 1);
    //FragColor = vec4(texture(u_Skybox, R).rgb, 1.0);
    FragColor = texture(u_Skybox, v_TexCoord);
}
