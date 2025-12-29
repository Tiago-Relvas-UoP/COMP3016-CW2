//STD
#include <iostream>

//GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp> //Access to the value_ptr
#include <glm/gtc/matrix_transform.hpp>

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//LEARNOPENGL
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

//GENERAL
#include "main.h"

//FASTNOISE
#include "FastNoiseLite.h"

using namespace std;
using namespace glm;

//Window
int windowWidth;
int windowHeight;

//VAO vertex attribute positions in correspondence to vertex attribute type
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 };
//VAOs
GLuint VAOs[NumVAOs];

//Buffer types
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
//Buffer objects
GLuint Buffers[NumBuffers];

//Transformations
//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//Camera sidways rotation
float cameraYaw = -90.0f;
//Camera vertical rotation
float cameraPitch = 0.0f;
//Determines if first entry of mouse into window
bool mouseFirstEntry = true;
//Positions of camera from given last frame
float cameraLastXPos = 800.0f / 2.0f;
float cameraLastYPos = 600.0f / 2.0f;

//Model-View-Projection Matrix
mat4 mvp;
mat4 model;
mat4 view;
mat4 projection;

//Time
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

// Terrain/Clouds
#define RENDER_DISTANCE 128 //Render width of map
#define MAP_SIZE RENDER_DISTANCE * RENDER_DISTANCE // Size of map in x & z space

const int squaresRow = RENDER_DISTANCE - 1; // Amount of chunks across one dimension
const int trianglesPerSquare = 2; // Two triangles per square to form a 1x1 chunk
const int trianglesGrid = squaresRow * squaresRow * trianglesPerSquare; // Amount of triangles on map

GLuint terrainVAO, terrainVBO, terrainEBO; // Terrain VAO, VBO and EBO

int main()
{
    //Initialisation of GLFW
    glfwInit();
    //Initialisation of 'GLFWwindow' object
    windowWidth = 1280;
    windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Lab5", NULL, NULL);

    //Checks if window has been successfully instantiated
    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        return -1;
    }

    //Sets cursor to automatically bind to window & hides cursor pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Binds OpenGL to window
    glfwMakeContextCurrent(window);

    //Initialisation of GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD failed to initialise\n";
        return -1;
    }

    //Loading of shaders
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    Shader TerrainShaders("shaders/terrainVertexShader.vert", "shaders/terrainFragmentShader.frag");
    Model Rock("media/rock/Rock07-Base.obj");
    Model Tree("media/tree/palm.obj");
    Shaders.use();

    // Assigning perlin noise type for map
    FastNoiseLite TerrainNoise;
    // Setting noise type for Perlin
    TerrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    // Sets the noise scale
    TerrainNoise.SetFrequency(0.05f);
    // Generates a random seed between integers 0 & 100
    int terrainSeed = rand() % 100;
    // Sets seed for noise
    TerrainNoise.SetSeed(terrainSeed);

    // Biome Noise
    FastNoiseLite BiomeNoise;
    BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    BiomeNoise.SetFrequency(0.05f);
    int biomeSeed = rand() % 100;
    BiomeNoise.SetSeed(biomeSeed);

    // Generation of height map vertices
    GLfloat terrainVertices[MAP_SIZE][6];

    // Terrain vertice index
    int i = 0;

    for (int y = 0; y < RENDER_DISTANCE; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            // Setting of height from 2D noise value at respective x & y coordinate
            terrainVertices[i][1] = TerrainNoise.GetNoise((float)x, (float)y * 1.0f);

            // Retrieval of biome to set
            float biomeValue = BiomeNoise.GetNoise((float)x, (float)y);

            if (biomeValue <= -0.75f) // Plains
            {
                terrainVertices[i][3] = 1.0f;
                terrainVertices[i][4] = 1.0f;
                terrainVertices[i][5] = 1.0f;
            }
            else { // Desert
                terrainVertices[i][3] = 0.8f;
                terrainVertices[i][4] = 0.8f;
                terrainVertices[i][5] = 0.8f;
            }

            i++;
        }
    }

    // Positions to start drawing from
    float drawingStartPosition = 1.0f;
    float columnVerticesOffset = drawingStartPosition;
    float rowVerticesOffset = drawingStartPosition;

    int rowIndex = 0;

    for (int i = 0; i < MAP_SIZE; i++)
    {
        // Generation of x & z vertices for horizontal plane
        terrainVertices[i][0] = columnVerticesOffset;
        terrainVertices[i][2] = rowVerticesOffset;

        // Shifts x position across for next triangle along grid
        columnVerticesOffset = columnVerticesOffset + -0.0625f;

        // Indexing of each chunk within row
        rowIndex++;

        // True when all triangles of the current row have been generated
        if (rowIndex == RENDER_DISTANCE)
        {
            // Resets for next row of triangles
            rowIndex = 0;
            // Resets x position for next row of triangles
            columnVerticesOffset = drawingStartPosition;
            // Shifts y position
            rowVerticesOffset = rowVerticesOffset + -0.0625f;
        }
    }

    //Generation of height map indices
    GLuint terrainIndices[trianglesGrid][3];

    //Positions to start mapping indices from
    int columnIndicesOffset = 0;
    int rowIndicesOffset = 0;

    // Generation of map indices in the form of chunks (1x1 right angle triangle squares)
    rowIndex = 0;
    for (int i = 0; i < trianglesGrid - 1; i += 2)
    {
        terrainIndices[i][0] = columnIndicesOffset + rowIndicesOffset; // top left
        terrainIndices[i][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; // bottom left
        terrainIndices[i][1] = 1 + columnIndicesOffset + rowIndicesOffset; // top right

        terrainIndices[i + 1][0] = 1 + columnIndicesOffset + rowIndicesOffset; // top right
        terrainIndices[i + 1][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; // bottom left
        terrainIndices[i + 1][1] = 1 + RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; // bottom right

        // Shifts x position across for next chunk along grid
        columnIndicesOffset = columnIndicesOffset + 1;

        // Indexing of each chunk within row
        rowIndex++;

        // true when all chunks of the current row have been generated
        if (rowIndex == squaresRow)
        {
            // Resets for next row of chunks
            rowIndex = 0;

            // Resets x position for next row of chunks
            columnIndicesOffset = 0;

            // Shifts y position
            rowIndicesOffset = rowIndicesOffset + RENDER_DISTANCE;
        }
    }

    //Sets index of VAO
    glGenVertexArrays(1, &terrainVAO);//TerrainVAO

    //Sets indexes of all required buffer objects
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    //Binds VAO to a buffer
    glBindVertexArray(terrainVAO);

    //Binds vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    //Allocates buffer memory for the vertices of the "Terrain" buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

    //Binding & allocation for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainIndices), terrainIndices, GL_STATIC_DRAW);

    //Allocation & indexing of vertex attribute memory for vertex shader
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Colours
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Unbinding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, 1280, 720);

    //Depth Testing
    glEnable(GL_DEPTH_TEST);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Sets the mouse_callback() function as the callback for the mouse movement event
    glfwSetCursorPosCallback(window, mouse_callback);

    //Render loop
    while (glfwWindowShouldClose(window) == false)
    {
        //Time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Input
        ProcessUserInput(window); //Takes user input

        //Rendering
        glClearColor(0.25f, 0.0f, 1.0f, 1.0f); //Colour to display on cleared window
        glClear(GL_COLOR_BUFFER_BIT); //Clears the colour buffer
        glClear(GL_DEPTH_BUFFER_BIT); //Might need

        glDisable(GL_CULL_FACE); //Discards all back-facing triangles

        //Draw Terrain
        TerrainShaders.use();
        mat4 terrainModel = mat4(1.0f);
        terrainModel = scale(terrainModel, vec3(2.0f, 2.0f, 2.0f));
        terrainModel = rotate(terrainModel, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
        terrainModel = translate(terrainModel, vec3(0.0f, -2.f, -1.5f));

        mat4 terrainMVP = projection * view * terrainModel;
        TerrainShaders.setMat4("mvpIn", terrainMVP);

        glBindVertexArray(terrainVAO);
        glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //Switch back to model shader
        Shaders.use();

        model = mat4(1.0f); //Model matrix
        model = scale(model, vec3(0.025f, 0.025f, 0.025f)); //rock //Scaling to zoom in
        model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f)); //Looking straight forward
        model = translate(model, vec3(0.0f, -2.f, -1.5f)); //Elevation to look upon terrain

        //Projection matrix
        projection = perspective(radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

        //Transformations & Drawing
        //Viewer orientation
        view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction

        //Rock
        SetMatrices(Shaders);
        Rock.Draw(Shaders);

        //Tree (changes MVP in relation to past values)
        model = mat4(1.0f); //Model matrix
        model = scale(model, vec3(0.20f, 0.20f, 0.20f)); //rock //Scaling to zoom in
        model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f)); //Looking straight forward
        model = translate(model, vec3(0.0f, -2.f, -1.5f)); //Elevation to look upon terrain

        //Projection matrix
        projection = perspective(radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

        SetMatrices(Shaders);
        Tree.Draw(Shaders);

        //Rock (reorient MVP back to starting values)
        model = scale(model, vec3(20.0f, 20.0f, 20.0f));
        SetMatrices(Shaders);

        //Refreshing
        glfwSwapBuffers(window); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events
    }

    //Safely terminates GLFW
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Resizes window based on contemporary width & height values
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        cameraLastXPos = (float)xpos;
        cameraLastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    //Sets values for change in position since last frame to current frame
    float xOffset = (float)xpos - cameraLastXPos;
    float yOffset = cameraLastYPos - (float)ypos;

    //Sets last positions to current positions for next frame
    cameraLastXPos = (float)xpos;
    cameraLastYPos = (float)ypos;

    //Moderates the change in position based on sensitivity value
    const float sensitivity = 0.025f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Adjusts yaw & pitch values against changes in positions
    cameraYaw += xOffset;
    cameraPitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (cameraPitch > 89.0f)
    {
        cameraPitch = 89.0f;
    }
    else if (cameraPitch < -89.0f)
    {
        cameraPitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    cameraFront = normalize(direction);
}

void ProcessUserInput(GLFWwindow* WindowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(WindowIn, true);
    }

    //Extent to which to move in one instance
    const float movementSpeed = 1.0f * deltaTime;
    //WASD controls
    if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
}

void SetMatrices(Shader& ShaderProgramIn)
{
    mat4 mvp = projection * view * model; //Setting of MVP
    ShaderProgramIn.setMat4("mvpIn", mvp); //Setting of uniform with Shader class
}