#shader vertex
#version 450 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Binormal;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec2 a_TexCoord;

out VertexDataOut 
{
    vec3 Pos;
    vec3 Normal;
    vec2 TexCoord;
} VertexOut;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    vec3 WorldPosition = vec3(u_Model * vec4(a_Position, 1.0));
    gl_Position = u_ViewProjection * vec4(WorldPosition, 1.0);

    VertexOut.Pos = WorldPosition;
    VertexOut.Normal = a_Normal;
    //VertexOut.Normal = normalize(mat3(u_Model) * a_Normal); /* Transform normals to world space. */
    VertexOut.TexCoord = a_TexCoord;
}


#shader fragment
#version 450 core
out vec4 FragColor;

in VertexDataOut
{
    vec3 Pos;
    vec3 Normal;
    vec2 TexCoord;
} VertexOut;

uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform sampler2D u_Normal1;

uniform sampler2D u_Texture;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

uniform float u_SpecularStrength;
uniform bool u_LightningEnabled;

void main()
{    
    //vec3 LightPos = VertexOut.Pos;
    vec3 LightPos = u_LightPos;

    vec4 DiffuseSample = texture(u_Diffuse1, VertexOut.TexCoord);
    vec4 SpecularSample = texture(u_Specular1, VertexOut.TexCoord);

    /* Sample the normal map and convert it from [0,1] to [-1,1] */
    vec3 NormalMap = texture(u_Normal1, VertexOut.TexCoord).rgb * 2.0 - 1.0;
    vec3 Normal = normalize(NormalMap);

    vec3 LightDir = normalize(LightPos - VertexOut.Pos);
    //vec3 LightDir = normalize(u_LightPos - VertexOut.Pos);
    float Diff = max(dot(Normal, LightDir), 0.0);
    vec3 Diffuse = Diff * DiffuseSample.rgb;

    vec3 ViewDirection = normalize(-VertexOut.Pos); 
    vec3 ReflectDirection = reflect(-LightDir, Normal);
    float Spec = pow(max(dot(ViewDirection, ReflectDirection), 0.0), SpecularSample.r * 256.0);

    /* Assuming white light for specular highlights. */
    vec3 Specular = u_SpecularStrength * Spec * u_LightColor;

    vec3 Ambient = 0.10 * u_LightColor;

    vec3 FinalColor = Ambient + Diffuse + Specular;

    if (u_LightningEnabled)
    {
        FragColor = vec4(FinalColor, 1.0); 
        FragColor.x -= u_SpecularStrength;
    }
    else
    {
        FragColor = texture(u_Texture, VertexOut.TexCoord);
    }
}
