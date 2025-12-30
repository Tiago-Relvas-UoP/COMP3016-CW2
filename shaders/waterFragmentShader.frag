#version 460
//Colour value to send to next stage
out vec4 FragColor;
// Colour coordinates from last stage
in vec3 colourFrag;
// Texture coordinatse from last stage
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    //Setting of colour coordinates to colour map
    //FragColor = vec4(colourFrag, 1.0f);
    //FragColor = texture(texture1, TexCoord) * vec4(colourFrag, 1.0f);
    
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);
}