#shader vertex
#version 450 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

out vec3 v_Pos;
out vec2 v_TexCoords;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewProjectionMatrix;

void main()
{
    // Pass through the normal and texture coordinates
    v_Normal = normalize(mat3(u_Model) * a_Normal); // Transform normals to world space
    v_TexCoords = a_TexCoords;

    // Calculate position in world space
    vec3 worldPosition = vec3(u_Model * vec4(a_Pos, 1.0));

    // Calculate the final position of the vertex
    //gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(worldPosition, 1.0);
    gl_Position = u_ViewProjectionMatrix * vec4(worldPosition, 1.0);
    v_Pos = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
}


#shader fragment
#version 450 core
out vec4 FragColor;

in vec3 v_Pos; // World space position of the fragment/vertex
in vec3 v_Normal; // Normal vector in world space
in vec2 v_TexCoords; // Texture coordinates

uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform sampler2D u_Normal1;
uniform sampler2D u_Height1;


void main()
{    
    //vec3 LightPos = vec3(10, 5, 0); // Example light position, adjust as needed
    vec3 LightPos = v_Pos;

    vec4 diffuseSample = texture(u_Diffuse1, v_TexCoords);
    vec4 specularSample = texture(u_Specular1, v_TexCoords);

    // Sample the normal map and convert it from [0,1] to [-1,1]
    vec3 normalMap = texture(u_Normal1, v_TexCoords).rgb * 2.0 - 1.0;
    vec3 normal = normalize(normalMap); // Assuming the normals are in tangent space

    vec3 lightDir = normalize(LightPos - v_Pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseSample.rgb;

    // Assuming the camera/view position is at the origin for simplicity
    vec3 viewDir = normalize(-v_Pos); 
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularSample.r * 256.0); // Specular intensity
    vec3 specular = vec3(1.0) * spec; // Assuming white light for specular highlights

    vec3 ambient = 0.1 * diffuseSample.rgb; // Adding an ambient term for basic ambient lighting

    vec3 FinalColor = ambient + diffuse + specular;

    FragColor = vec4(FinalColor, 1.0); // Use calculated lighting color
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Use calculated lighting color
}
