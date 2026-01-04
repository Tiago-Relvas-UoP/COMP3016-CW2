#version 460
//Colour value to send to next stage
out vec4 FragColor;

//Texture coordinates from last stage
in vec2 textureFrag;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    // normals calculation using derivatives
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 lighting = ambient + diffuse;
    vec4 texColor = texture(texture_diffuse1, textureFrag);
    
    //Setting of colour coordinates to colour map
    FragColor = vec4(lighting * texColor.rgb, texColor.a);

    //Setting of colour coordinates to colour map
    //FragColor = texture(texture_diffuse1, textureFrag);
}