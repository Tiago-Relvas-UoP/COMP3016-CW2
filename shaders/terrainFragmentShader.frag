#version 460
//Colour value to send to next stage
out vec4 FragColor;

// Colour coordinates from last stage
in vec3 colourFrag;

in vec3 FragPos;

// Light Color
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{

    // normals calculation using derivatives
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * colourFrag;

    //Setting of colour coordinates to colour map
    FragColor = vec4(result, 1.0f);
}