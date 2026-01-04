#version 460
//Triangle position with values retrieved from main.cpp
layout (location = 0) in vec3 position;
//Texture coordinates from last stage
layout (location = 2) in vec2 textureVertex;

//Model-View-Projection Matrix
uniform mat4 mvpIn;
uniform mat4 model; // Model Matrix

//Outputs (Texture/Fragment pos)
out vec2 textureFrag;
out vec3 FragPos;

void main()
{
    //Calculates Fragment Position in scene
    FragPos = vec3(model * vec4(position, 1.0));
    //Transformation applied to vertices
    gl_Position = mvpIn * vec4(position, 1.0);
    //Sending texture coordinates to next stage
    textureFrag = textureVertex;
}