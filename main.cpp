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

//IRRKLANG
#include "irrKlang.h"

//namespaces
using namespace std;
using namespace glm;
using namespace irrklang;

//Window dimensions
int windowWidth = 1280;
int windowHeight = 720;

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

// FOV value + nearPlane/farPlane value for clipping purposes
const float fov = 90.0f;
const float nearPlane = 0.1f;
const float farPlane = 100.0f;

//Model-View-Projection Matrix
mat4 mvp;
mat4 model;
mat4 view;
mat4 projection;

//TIME VARIABLES
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

// UFO VARIABLES
const vec3 ufoPosition = vec3(-9.0f, -1.0f, -9.0f);
const float ufoScale = 0.07f;
const float ufoInitialRotation = 90.0f;

// PLANE VARIABLES
const vec3 planePosition = vec3(-2.5f, -1.4f, -9.0f);
const float planeScale = 0.025f;

float bobSpeed = 1.0f; // Controls how fast the plane bobs
float bobAmount = 0.3f; // Controls vertical movement distance
float tiltAmount = 2.0f; // Controls tilt angle in degrees

// Procedurally-generated Terrain (clouds) config
#define RENDER_DISTANCE 128 //Render width of map
#define MAP_SIZE RENDER_DISTANCE * RENDER_DISTANCE // Size of map in x & z space

const int squaresRow = RENDER_DISTANCE - 1; // Amount of chunks across one dimension
const int trianglesPerSquare = 2; // Two triangles per square to form a 1x1 chunk
const int trianglesGrid = squaresRow * squaresRow * trianglesPerSquare; // Amount of triangles on map
const float whiteColorThreshold = -0.75f; // Height Threshold for White Color
const float greyColorThreshold = -0.50f; // Height Threshold for Grey Color

// Buffer Objects for Terrain (Clouds), Water and Sun/Light Objects
GLuint terrainVAO, terrainVBO, terrainEBO; // Terrain VAO, VBO and EBO
GLuint waterVAO, waterVBO, waterEBO; // Water VAO, VBO and EBO
GLuint sunVAO, sunVBO, lightVAO; // sun VAO, VBO & light VAO

// Main
int main()
{
    //Initialisation of GLFW
    glfwInit();
    //Initialisation of 'GLFWwindow' object
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Peaceful Coexistence - CW2", NULL, NULL);

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

    //Initialization of IRRKLANG Sound Engine
    ISoundEngine* engine = createIrrKlangDevice();
    if (!engine) {
        return 0;
    }

    //Shaders Loading  
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag"); // Main Shader (For models)
    Shader WaterShaders("shaders/waterVertexShader.vert", "shaders/waterFragmentShader.frag"); // Water Shader
    Shader TerrainShaders("shaders/terrainVertexShader.vert", "shaders/terrainFragmentShader.frag"); // Terrain (Clouds) Shader
    Shader LightShader("shaders/LightVertexShader.vert", "shaders/LightFragmentShader.frag"); // Light Shader
    
    // 3D Model Loading
    Model Ufo("media/ufo/model/Low_poly_UFO.FBX"); // Ufo FBX
    Model Plane("media/plane/floatplane/floatplane.obj"); // Plane OBJ

    // Audio loading
    engine->play2D("media/audio/ambience.wav", true); //Ambience Sound

    // Activates main shader
    Shaders.use();

    // !! CODE SEGMENT FOR TERRAIN (CLOUDS) GENERATION !! 

    // Terrain Noise
    FastNoiseLite TerrainNoise; // Assigning perlin noise type for map
    TerrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Setting noise type for Perlin
    TerrainNoise.SetFrequency(0.05f);  // Sets the noise scale
    TerrainNoise.SetSeed(rand() % 100); // Sets a random seed for noise (Between integers 0 & 100)

    // Biome Noise
    FastNoiseLite BiomeNoise; // Assigning celular noise type for map
    BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular); // Setting noise type for Cellular
    BiomeNoise.SetFrequency(0.05f); // Sets the noise scale
    BiomeNoise.SetSeed(rand() % 100);// Sets a random seed for noise (Between integers 0 & 100)

    // Generation of height map vertices
    GLfloat terrainVertices[MAP_SIZE][6];

    // Terrain vertice index
    int i = 0;

    // Generate height and color data for each vertex
    for (int y = 0; y < RENDER_DISTANCE; y++)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            // Setting of height from 2D noise value at respective x & y coordinate
            terrainVertices[i][1] = TerrainNoise.GetNoise((float)x, (float)y * 1.0f);

            // Retrieval of biome to set
            float biomeValue = BiomeNoise.GetNoise((float)x, (float)y);

            if (biomeValue <= whiteColorThreshold) // White Color
            {
                terrainVertices[i][3] = 1.0f;
                terrainVertices[i][4] = 1.0f;
                terrainVertices[i][5] = 1.0f;
            }
            else if (biomeValue <= greyColorThreshold){ // Grey Color
                terrainVertices[i][3] = 0.8f;
                terrainVertices[i][4] = 0.8f;
                terrainVertices[i][5] = 0.8f;
            }
            else { // Dark Grey Color
                terrainVertices[i][3] = 0.6f;
                terrainVertices[i][4] = 0.6f;
                terrainVertices[i][5] = 0.6f;
            }

            i++;
        }
    }

    // Positions to start drawing from
    float drawingStartPosition = 1.0f;
    float columnVerticesOffset = drawingStartPosition;
    float rowVerticesOffset = drawingStartPosition;

    int rowIndex = 0;

    // Assigns x & z positions to create the terrains horizontal grid
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

    // !! END CODE SEGMENT FOR TERRAIN (CLOUDS) GENERATION !! 
    
    // !! TERRAIN (CLOUDS) BUFFER SETUP !! 

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

    // !! END OF TERRAIN (CLOUDS) BUFFER SETUP !! 

    // !! WATER VERTEX, INDICES AND BUFFER SETUP !! 

    //Water vertices (Squashed down Cube)
    float waterVertices[] = {
        // Front face
        //Positions                     //Colours (Blue)        //Texture
        1.0f, -3.0f, 1.0f,              0.0f, 0.5f, 1.0f,       1.0f, 1.0f,  // 0: top right front
        1.0f, -3.5f, 1.0f,              0.0f, 0.4f, 0.9f,       1.0f, 0.0f,  // 1: bottom right front
        -6.9375f, -3.5f, 1.0f,          0.0f, 0.4f, 0.9f,       0.0f, 0.0f,  // 2: bottom left front
        -6.9375f, -3.0f, 1.0f,          0.0f, 0.5f, 1.0f,       0.0f, 1.0f,  // 3: top left front

        // Back face      
        //Positions                     //Colours (Blue)        //Texture
        1.0f, -3.0f, -6.9375f,          0.0f, 0.5f, 1.0f,       1.0f, 1.0f,  // 4: top right back
        1.0f, -3.5f, -6.9375f,          0.0f, 0.4f, 0.9f,       1.0f, 0.0f,  // 5: bottom right back
        -6.9375f, -3.5f, -6.9375f,      0.0f, 0.4f, 0.9f,       0.0f, 0.0f,  // 6: bottom left back
        -6.9375f, -3.0f, -6.9375f,      0.0f, 0.5f, 1.0f,       0.0f, 1.0f,   // 7: top left back

        // Top face 
        //Positions                     //Colours (Blue)        //Texture
        1.0f, -3.0f, 1.0f,              0.0f, 0.5f, 1.0f,       1.0f, 0.0f,  // 8: top right front
        -6.9375f, -3.0f, 1.0f,          0.0f, 0.5f, 1.0f,       0.0f, 0.0f,  // 9: top left front
        1.0f, -3.0f, -6.9375f,          0.0f, 0.5f, 1.0f,       1.0f, 1.0f,  // 10: top right back
        -6.9375f, -3.0f, -6.9375f,      0.0f, 0.5f, 1.0f,       0.0f, 1.0f,  // 11: top left back

        // Bottom face         //Colours (Blue)        //Texture
        1.0f, -3.5f, 1.0f,              0.0f, 0.4f, 0.9f,       1.0f, 0.0f,  // 12:bottom right front
        -6.9375f, -3.5f, 1.0f,          0.0f, 0.4f, 0.9f,       0.0f, 0.0f,  // 13: bottom left front
        1.0f, -3.5f, -6.9375f,          0.0f, 0.4f, 0.9f,       1.0f, 1.0f,  // 14: bottom right back
        -6.9375f, -3.5f, -6.9375f,      0.0f, 0.4f, 0.9f,       0.0f, 1.0f   // 15: bottom left back
    };

    // Water Indices
    unsigned int waterIndices[] = {
        // Front face
        0, 1, 3,
        1, 2, 3,
        // Back face
        4, 7, 5,
        5, 7, 6,
        // Top face 
        8, 9, 10,
        9, 11, 10,
        // Bottom face
        12, 14, 13,
        13, 14, 15,
        // Left face
        3, 2, 7,
        2, 6, 7,
        // Right face
        0, 4, 1,
        1, 4, 5
    };

    //Sets index of water VAO
    glGenVertexArrays(1, &waterVAO);
    //Sets indexes of water buffer objects
    glGenBuffers(1, &waterVBO);
    glGenBuffers(1, &waterEBO);

    //Binds water VAO to a buffer
    glBindVertexArray(waterVAO);

    //Binds water vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    //Allocates buffer memory for the vertices of the water buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);

    //Binding & allocation for water indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(waterIndices), waterIndices, GL_STATIC_DRAW);

    //Allocation & indexing of vertex attribute memory for water vertex shader
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Colours
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //Unbinding
    glBindVertexArray(0);

    // !! END OF WATER VERTEX, INDICES AND BUFFER SETUP !! 


    // !! SUN/LIGHT VERTEX, INDICES AND BUFFER SETUP !! 
 
    //sun vertices (Cube)
    float sunVertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    //Sets index of sun VAO
    glGenVertexArrays(1, &sunVAO);

    //Sets indexes of sun buffer object
    glGenBuffers(1, &sunVBO);
    
    //Binds sun VAO to a buffer
    glBindVertexArray(sunVAO);
    //Binds sun vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    //Allocates buffer memory for the vertices of the sun buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunVertices), sunVertices, GL_STATIC_DRAW);

    // Allocation & indexing of vertex attribute memory for sun vertex shader
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Sets index of light VAO
    glGenVertexArrays(1, &lightVAO);
    //Binds light VAO to a buffer
    glBindVertexArray(lightVAO);

    //Binds sun vertex object to array buffer
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);

    // Allocation & indexing of vertex attribute memory for light vertex shader
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Unbinding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // !! END OF SUN/LIGHT VERTEX, INDICES AND BUFFER SETUP !!

    unsigned int texture1, texture2;

    // WATER TEXTURE (1ST)
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("media/textures/seamlesswater.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout << "Successfully loaded texture" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // SIGNATURE TEXTURE (2ND)
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    data = stbi_load("media/textures/water2.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout << "Successfully loaded texture" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Sets the mouse_callback() function as the callback for the mouse movement event
    glfwSetCursorPosCallback(window, mouse_callback);

    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, 1280, 720);

    //Depth Testing
    glEnable(GL_DEPTH_TEST);

    //Render loop
    while (glfwWindowShouldClose(window) == false)
    {
        // !! TIME !!
        // Calculates delta time so movement is not dependant on frames
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // !! INPUT PROCESSING !!
        ProcessUserInput(window); //Takes user input

        // !! RENDERING !!
        glClearColor(0.00, 0.05, 0.17, 1.0); //Colour to display on cleared window // Blue: (0.25f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); //Clears the colour buffer
        glClear(GL_DEPTH_BUFFER_BIT); //Clears depth buffer for proper 3D rendering

        glDisable(GL_CULL_FACE); //Discards all back-facing triangles

        // !! LIGHT POSITION / COLOR !! 
        vec3 lightPosition = vec3(0.0f, 5.0f, 0.0f);
        vec3 lightCol = vec3(0.8f, 0.8f, 0.8f);

        // !! DRAW TERRAIN !!
        TerrainShaders.use();
        TerrainShaders.setVec3("lightColor", lightCol);
        TerrainShaders.setVec3("lightPos", lightPosition); 
        TerrainShaders.setVec3("viewPos", cameraPosition);

        //Current Time for animation
        float time = static_cast<float>(glfwGetTime());
        //Time Scale (How fast the Terrain (clouds) change)
        float timeScale = 1.0f;

        // Regenerate height values with time offset
        int i = 0;
        for (int y = 0; y < RENDER_DISTANCE; y++)
        {
            for (int x = 0; x < RENDER_DISTANCE; x++)
            {
                // Add time as third dimension to create smooth movement
                terrainVertices[i][1] = TerrainNoise.GetNoise((float)x, (float)y, time * timeScale);
                i++;
            }
        }

        // Updates Terrain (Clouds) VBO with new data
        glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(terrainVertices), terrainVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Terrain (clouds) model matrix transformations
        mat4 terrainModel = mat4(1.0f);
        terrainModel = scale(terrainModel, vec3(2.0f, 2.0f, 2.0f));
        terrainModel = rotate(terrainModel, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
        terrainModel = translate(terrainModel, vec3(0.0f, -2.0f, -1.5f));

        // Calculates and sets MVP matrix for terrain (clouds)
        mat4 terrainMVP = projection * view * terrainModel;
        TerrainShaders.setMat4("mvpIn", terrainMVP);
        TerrainShaders.setMat4("model", terrainModel); 

        // Draws terrain using indexed rendering
        glBindVertexArray(terrainVAO);
        glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // !! DRAW WATER !!
        WaterShaders.use();
        WaterShaders.setVec3("lightColor", lightCol);
        WaterShaders.setVec3("lightPos", lightPosition);

        // Sets texture uniforms for water model (Water/Signature texture)
        glUniform1i(glGetUniformLocation(WaterShaders.ID, "texture1"), 0);
        WaterShaders.setInt("texture2", 1);

        // Water model matrix transformations
        mat4 waterModel = mat4(1.0f);
        waterModel = scale(waterModel, vec3(2.0f, 2.0f, 2.0f));
        waterModel = rotate(waterModel, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
        waterModel = translate(waterModel, vec3(0.0f, -2.0f, -1.5f));

        // Calculates and sets MVP matrix for water
        mat4 waterMVP = projection * view * waterModel;
        WaterShaders.setMat4("mvpIn", waterMVP);
        WaterShaders.setMat4("model", waterModel); 

        // Binds textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // Draws water using indexed rendering
        glBindVertexArray(waterVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // !! DRAW SUN/LIGHT CUBE !!
        LightShader.use();

        // Suns model matrix transformations
        mat4 lightModel = mat4(1.0f);
        lightModel = translate(lightModel, lightPosition);
        lightModel = scale(lightModel, vec3(0.2f));

        // Calculates and sets MVP matrix for sun/light
        mat4 lightMVP = projection * view * lightModel;
        LightShader.setMat4("mvpIn", lightMVP);

        // Draws sun/light cube using array rendering
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // !! MODEL DRAWING !!

        //Switch back to model shader
        Shaders.use();
        Shaders.setVec3("lightColor", lightCol);
        Shaders.setVec3("lightPos", lightPosition);

        // !! UFO MODEL !!

        //Projection matrix
        projection = perspective(radians(fov), (float)windowWidth / (float)windowHeight, nearPlane, farPlane);

        //Transformations & Drawing
        //Viewer orientation
        view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction

        // Ufo model matrix transformations
        model = mat4(1.0f); //Model matrix
        model = translate(model, ufoPosition); // World Position
        model = rotate(model, radians(ufoInitialRotation), vec3(-1.0f, 0.0f, 0.0f)); // Initial rotation
        model = rotate(model, (float)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f)); // Continous rotation over time in the Z-Axis
        model = scale(model, vec3(ufoScale)); // Scaled down to appropriate size (1.0f is too large)

        // Draws UFO after setting matrices 
        SetMatrices(Shaders);
        Shaders.setMat4("model", model); 
        Ufo.Draw(Shaders);

        // !! PLANE MODEL !! 

        // Position/Rotation variables. Will determine how much the plane should move in the Y-Axis, and how much rotation should be applied in the X-Axis.
        float bobOffset = sin((float)glfwGetTime() * bobSpeed) * bobAmount;
        float tiltAngle = sin((float)glfwGetTime() * bobSpeed) * tiltAmount;

        // Plane model matrix transformations
        model = mat4(1.0f); //Model matrix
        model = translate(model, planePosition + vec3(0.0f, bobOffset, 0.0f)); // World Position with vertical bobbing
        model = rotate(model, radians(tiltAngle), vec3(1.0f, 0.0f, 0.0f)); // Pitching motion
        model = scale(model, vec3(planeScale)); // Scaled down to appropriate size (1.0f is too large)

        // Draws Plane after setting matrices 
        SetMatrices(Shaders);
        Shaders.setMat4("model", model);
        Plane.Draw(Shaders);

        // !! FRAME FINALIZATION !! 

        //Refreshing
        glfwSwapBuffers(window); //Swaps the colour buffer
        glfwPollEvents(); //Queries all GLFW events (Mouse, keyboard, etc.)
    }

    //Clean up audio after program closes
    engine->drop();

    //Safely terminates GLFW
    glfwTerminate();
    return 0;
}

// Callback function that allows window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Resizes window based on contemporary width & height values
    glViewport(0, 0, width, height);
}

// Callback function that allows mouse movement in first person
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

// Callback function that allows movement to be processed by keyboard
void ProcessUserInput(GLFWwindow* WindowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(WindowIn, true);
    }

    //Extent to which to move in one instance
    const float movementSpeed = 5.0f * deltaTime;
    //WASD controls
    if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS) // W Key
    {
        cameraPosition += movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS) // S Key
    {
        cameraPosition -= movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS) // A Key
    {
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS) // D Key
    {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
}

// Function that sets MVP matrices for Shader use
void SetMatrices(Shader& ShaderProgramIn)
{
    mat4 mvp = projection * view * model; //Setting of MVP
    ShaderProgramIn.setMat4("mvpIn", mvp); //Setting of uniform with Shader class
}