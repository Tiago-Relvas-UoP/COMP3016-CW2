#version 460

//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 position;
//Colour coordinates from vertex data
layout (location = 1) in vec3 colourVertex;
//Texture coordinates from vertex data
layout (location = 2) in vec2 aTexCoord;

//Model-View-Projection Matrix
uniform mat4 mvpIn;
uniform mat4 model;

//Outputs
// Fragment Position to send
out vec3 FragPos;
// Colour to send
out vec3 colourFrag;
// Texture to send
out vec2 TexCoord;

void main()
{
    // Calculate fragment position in world space
    FragPos = vec3(model * vec4(position, 1.0));
    // Transformation applied to vertices
    gl_Position = mvpIn * vec4(position, 1.0);
    // Sending colour coordinates to next stage
    colourFrag = colourVertex;
    // Sending texture coordinates to next stage
    TexCoord = aTexCoord;

}