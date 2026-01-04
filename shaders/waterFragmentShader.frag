#version 460
//Colour value to send to next stage
out vec4 FragColor;
// Fragment position
in vec3 FragPos;
// Colour coordinates from last stage
in vec3 colourFrag;
// Texture coordinatse from last stage
in vec2 TexCoord;

// Texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

// Lighting uniforms
uniform vec3 lightPos;     
uniform vec3 lightColor;    

void main()
{
    // normals calculation using derivatives
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));

    // Base Ambient Lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse Lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Texture mixing
    vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

    //Setting of colour coordinates to colour map
    vec3 lighting = ambient + diffuse;
    vec3 result = lighting * texColor.rgb * colourFrag;
    FragColor = vec4(result, texColor.a); // Final Output
    
    //vec4 texColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

    //Setting of colour coordinates to colour map
    //FragColor = vec4(colourFrag, 1.0f);
    //FragColor = texture(texture1, TexCoord) * vec4(colourFrag, 1.0f);
    
    //UNCOMMENT THIS INCASE OF ERROR
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);
}