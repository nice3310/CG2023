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

//camera position ( for specular )
uniform vec3 cameraPos;

//Map
uniform sampler2D mapKd; //new

// Data received from vertex shader.
in vec3 fsPosition;
in vec3 fsNormal;
in vec2 iTexCoord; 

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


void main()
{

    // Normalize the normal vector in world space.
    vec3 N = normalize(fsNormal);

    // View direction in world space.
    vec3 V = normalize (cameraPos - fsPosition); 

    // Ambient light.
    vec3 ambient = Ka * ambientLight;

    // Directional light.
    vec3 L = normalize(-normalize(dirLightDir));
    vec3 diffuse = Diffuse(Kd, dirLightRadiance, N, L);
    vec3 specular = Specular(Ks,dirLightRadiance, Ns, V, N, L);
    vec3 dirLight = diffuse + specular;


    //vec3 lighting = dirLight;
    vec3 lighting = dirLight;

    vec3 texColor = texture2D(mapKd, iTexCoord).rgb;//new

    FragColor = vec4(lighting * texColor, 1.0); //new
    //FragColor = vec4(dirLight, 1.0); //new
    //FragColor = vec4(texColor, 1.0); //new
    //FragColor = vec4(lighting, 1.0);

    
}
