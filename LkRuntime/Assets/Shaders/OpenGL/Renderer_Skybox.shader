#shader vertex
#version 450 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_Texcoord;

//out vec3 v_Pos;
//out vec3 v_Normal;
out vec3 v_Texcoord;

uniform mat4 u_Model;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewProjectionMatrix;

void main()
{
    vec3 WorldPos = vec3(u_Model * vec4(a_Position, 1.0));
    gl_Position = u_ViewProjectionMatrix * vec4(WorldPos, 1.0);

    //gl_Position = u_ViewProjectionMatrix * vec4(a_Position, 1.0);
    //v_Texcoord = vec3(a_Position.xy, -a_Position.z);
    v_Texcoord = a_Position;
}


#shader fragment
#version 450 core
out vec4 FragColor;

//in vec3 v_Pos; // World space position of the fragment/vertex
//in vec3 v_Normal; // Normal vector in world space
in vec3 v_Texcoord; // Texture coordinates

uniform vec3 u_CameraPos;
uniform samplerCube u_Skybox;

void main()
{    
    //vec3 I = normalize(v_Pos, u_CameraPos);
    //vec3 R = reflect(vec3(1, 1, 1), normalize(v_Normal));
    //vec3 R = vec3(1, 1, 1);
    //FragColor = vec4(texture(u_Skybox, R).rgb, 1.0);
    //FragColor = vec4(1, 0, 1, 1);
    FragColor = texture(u_Skybox, v_Texcoord);
}
