#version 460
//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 position;
//Colour coordinates from vertex data
layout (location = 1) in vec3 colourVertex;
//Model-View-Projection Matrix
uniform mat4 mvpIn;
// Colour to send
out vec3 colourFrag;
void main()
{
    // Transformation applied to vertices
    gl_Position = mvpIn * vec4(position.x, position.y, position.z, 1.0);
    // Sending colour coordinates to next stage
    colourFrag = colourVertex;
}