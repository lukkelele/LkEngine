#shader vertex
#version 450 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Binormal;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec2 a_TexCoord;


out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProjectionMatrix;

void main()
{
    vec3 WorldPosition = vec3(u_Model * vec4(a_Position, 1.0));

    gl_Position = u_ViewProjectionMatrix * vec4(WorldPosition, 1.0);

    v_Position = gl_Position.xyz;
    v_Normal = normalize(mat3(u_Model) * a_Normal); // Transform normals to world space
    v_TexCoord = a_TexCoord;
}


#shader fragment
#version 450 core
out vec4 FragColor;

in vec3 v_Position;
in vec3 v_Normal;   // In world space
in vec2 v_TexCoord; 

uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform sampler2D u_Normal1;
uniform sampler2D u_Height1;

uniform sampler2D u_Texture0;


void main()
{    
    //vec3 LightPos = vec3(10, 5, 0); // Example Light position, adjust as needed
    vec3 LightPos = v_Position;

    vec4 DiffuseSample = texture(u_Diffuse1, v_TexCoord);
    vec4 SpecularSample = texture(u_Specular1, v_TexCoord);

    // Sample the normal map and convert it from [0,1] to [-1,1]
    vec3 normalMap = texture(u_Normal1, v_TexCoord).rgb * 2.0 - 1.0;
    vec3 normal = normalize(normalMap); // Assuming the normals are in tangent space

    vec3 LightDir = normalize(LightPos - v_Position);
    float Diff = max(dot(normal, LightDir), 0.0);
    vec3 Diffuse = Diff * DiffuseSample.rgb;

    // Assuming the camera/view position is at the origin for simplicity
    vec3 ViewDirection = normalize(-v_Position); 
    vec3 ReflectDirection = reflect(-LightDir, normal);
    float Spec = pow(max(dot(ViewDirection, ReflectDirection), 0.0), SpecularSample.r * 256.0); // Specular intensity
    vec3 Specular = vec3(1.0) * Spec; // Assuming white Light for Specular highLights
    vec3 Ambient = 0.1 * DiffuseSample.rgb; // Adding an ambient term for basic ambient Lighting
    vec3 FinalColor = Ambient + Diffuse + Specular;

    FragColor = texture(u_Texture0, v_TexCoord);
    //FragColor = vec4(FinalColor, 1.0); 
    //FragColor = vec4(1.0, 0.2, 0.3, 1.0);
}
