#version 460

//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 position;

//Colour coordinates from vertex data
layout (location = 1) in vec3 colourVertex;

//Model-View-Projection Matrix + Model
uniform mat4 mvpIn;
uniform mat4 model;

// Colour to send
out vec3 FragPos;
out vec3 colourFrag;

void main()
{
    // Calculate fragment position in world space
    FragPos = vec3(model * vec4(position, 1.0));
    // Transformation applied to vertices
    gl_Position = mvpIn * vec4(position, 1.0);
    // Sending colour coordinates to next stage
    colourFrag = colourVertex;
}