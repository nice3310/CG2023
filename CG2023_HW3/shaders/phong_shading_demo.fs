#version 330 core

// Material properties.
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
// Light data.
uniform vec3 ambientLight;
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;
uniform vec3 spotLightPosition;
uniform vec3 spotLightDirection;
uniform vec3 spotLightIntensity;
uniform float spotLightCutoffStartInDegree;
uniform float spotLightTotalWidthInDegree;

//camera position ( for specular )
uniform vec3 cameraPos;

//Map
uniform sampler2D mapKd; //new

// Data received from vertex shader.
in vec3 fsPosition;
in vec3 fsNormal;
in vec2 iTexCoord; //new


out vec4 FragColor;

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 Ks, vec3 I, float Ns, vec3 V, vec3 N, vec3 lightDir)
{
    vec3 R = reflect(-lightDir, N);
    return Ks * I * pow(max(dot(V, R), 0.0), Ns);
}

vec3 Spotlight(vec3 Kd, vec3 Ks, float Ns, vec3 V, vec3 N, vec3 spotPosition, vec3 spotDirection, vec3 intensity, float cutoffStartInDegree, float totalWidthInDegree)
{
    vec3 L = normalize(spotPosition - fsPosition);
    float dist = distance(spotPosition, fsPosition);
    float attenuation = 1.0f / (dist * dist);

    float theta = dot(L, normalize(-spotDirection));
    float epsilon = cos(radians(cutoffStartInDegree)) - cos(radians(totalWidthInDegree));
    float intensityFactor = smoothstep(cos(radians(totalWidthInDegree)), cos(radians(cutoffStartInDegree)), theta);

    vec3 spotlightDiffuse = Diffuse(Kd, intensity * attenuation, N, L);
    vec3 spotlightSpecular = Specular(Ks, intensity * attenuation, Ns, V, N, L);

    return (spotlightDiffuse + spotlightSpecular) * intensityFactor;
}


void main()
{
    // Normalize the normal vector in world space.
    vec3 N = normalize(fsNormal);

    // View direction in world space.
    vec3 V = normalize (cameraPos - fsPosition); 

    // Ambient light.
    vec3 ambient = Ka * ambientLight;

    // Directional light.
    vec3 L = normalize(-dirLightDir);
    vec3 diffuse = Diffuse(Kd, dirLightRadiance, N, L);
    vec3 specular = Specular(Ks,dirLightRadiance, Ns, V, N, L);
    vec3 dirLight = diffuse + specular;

    // Point light.
    L = normalize(pointLightPos - fsPosition);
    float dist = distance(pointLightPos, fsPosition);
    float attenuation = 1.0f / (dist * dist);
    vec3 radiance = pointLightIntensity * attenuation;

    diffuse = Diffuse(Kd, radiance, N, L);
    specular = Specular(Ks,radiance, Ns, V, N, L);
    vec3 pointLight = (diffuse + specular);

    //spot light .
    vec3 spotlight = Spotlight(Kd, Ks, Ns, V, N, spotLightPosition, spotLightDirection, spotLightIntensity, spotLightCutoffStartInDegree, spotLightTotalWidthInDegree);

    //vec3 lighting = dirLight;
    vec3 lighting = ambient + dirLight + pointLight + spotlight;

    vec3 texColor = texture2D(mapKd, iTexCoord).rgb;//new
    FragColor = vec4(lighting * texColor, 1.0); //new
    //FragColor = vec4(lighting, 1.0);

    
}
