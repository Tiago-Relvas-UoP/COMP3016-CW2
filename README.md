# PEACEFUL COEXISTENCE - CW2

## Youtube video
[Link](https://youtu.be/N0AhzRSZ5Kw)

## Instructions

Important: This project uses Visual Studio Community 2022 with "Desktop development with C++" Workload installed. A .exe installer can be found in this [repository](https://github.com/Tiago-Relvas-UoP/COMP3016-CW2/tree/main/vs22-setup) as a precaution, considering the official microsoft website no longer has a download for this version of VS.

### Installing Assimp
- All dependencies are already included in the source code apart from Assimp, due to its size.
- To install Assimp, create a folder name `Assimp` in `C:\Users\Public`, and two folders inside it named `Source` and `Binaries`
- Inside the `Source` folder, open a Terminal and type the following command: `git clone https://github.com/assimp/assimp.git` 
- Install [Cmake](https://cmake.org/download/). After installing, open it and at the top select `C:\Users\Public\Assimp\Source\assimp` as the location of the Source Code, and `C:\Users\Public\Assimp\Binaries` as the place to build the binaries.
- Afterwards, navigate to the bottom of the program and open the "Configure" menu. Select "Visual Studio 17 2022" as the generator and then click "Finish"
- Once the configuration is done, click on "Generate".
- After the generation is done, go to File Explorer and navigate to `C:\Users\Public\Assimp\Binaries`. Open the `Assimp.sln` file with VS2022.
- Once inside, at the top on the left of "x64", change from Debug to Release and then hit "Local Windows Debugger".
- This will generate all important libraries. Wait until it finishes building the solution, or until an error related to "\ALL_BUILD" Access is denied" appears.
- Assimp is now successfuly installed.

### Compiling the program (After installing Assimp)
- Download source code.
- Extract it anywhere.
- Open project file (`OpenGL-CW2.sln`) with with VS2022.
- At the top, select either Debug/Release and 64x/86x (Depending if your system is 64/32 bits).
- Next to those options, select "Local Windows Debugger".
- Enjoy!

## Project Description
"Peaceful coexistence" is a simple immersive 3D Scene built in OpenGL that explores a human and an alien peacefully flying in the middle of the ocean, above the clouds

This 3D Scene features:
- A 3D Water Surface that blends in 2 mixed textures, and a gradient Color
- Procedurally generated clouds using Perlin Noise that continously fluctuate over the runtime of the application
- 3D Animated Models: An UFO (with a signature), and a plane
- Static Lighting source (Sun) that illuminates all scene objects
- Fluid First-person movement (WASD/Camera)
- A relaxing Ambience music

## THIRD-PARTY ASSETS CREDITS (AND SPECIAL THANKS)
- Low Poly UFO (FBX model) by JonLundy3D (From [Turbosquid](https://www.turbosquid.com/pt_br/3d-models/free-3ds-model-flying-saucer/1081073))
- Floatplane (OBJ model) by HuNtEr_3DdD (From [Turbosquid](https://www.turbosquid.com/pt_br/3d-models/lowpoly-floatplane-pbr-3d-model-2446919))
- Ambience by kanaizo (from [Freesound](https://freesound.org/people/kanaizo/sounds/811838/))
- Seamless Water Texture from [CADHatch](https://www.cadhatch.com/seamless-water-textures)
- Seamless Water Texture 2 from [Jessica's Nature Blog](https://natureinfocus.blog/2019/06/18/water-studies-ham-geo-south-ronaldsay-062018/)
- Special thanks to the holy grail: [LearnOpenGL](https://learnopengl.com)

## Dependencies used.
This project used the following dependencies:
- GLAD: Essential Library for a bunch of OpenGL Functions.
- GLFW: Responsible for allowing Window Creationg, context management and all input handling.
- GLM: Math. Library responsible for all vector/matrix operations and transformations.

- Assimp: Responsible for loading all 3D Models within the scene that use OBJ/FBX format.
- stb_image: Responsible for loading images, used as textures.
- IrrKlang: Responsible for audio playback (Ambience Noise)
- FastNoiseLite: Responsible for providing Perlin Noise, used in the procedurally generated terrain (Clouds)

## Sample Screens
![alt text](https://github.com/Tiago-Relvas-UoP/COMP3016-CW2/blob/main/sample-screens/sample1.png)
![alt text](https://github.com/Tiago-Relvas-UoP/COMP3016-CW2/blob/main/sample-screens/sample2.png)
![alt text](https://github.com/Tiago-Relvas-UoP/COMP3016-CW2/blob/main/sample-screens/sample3.png)
![alt text](https://github.com/Tiago-Relvas-UoP/COMP3016-CW2/blob/main/sample-screens/sample4.png)

## Used Programming Pattern

### Global Configuration Pattern
- Multiple variables are declared in the global scope as to improve readability and enable easier value changes
- These variables include window dimensions, Render Distance (For the Clouds Terrain), Camera Settings, etc..

```cpp
// DEFINED BEFORE THE MAIN LOOP
//Window dimensions
int windowWidth = 1280;
int windowHeight = 720;

// Procedurally-generated Terrain (clouds) config
#define RENDER_DISTANCE 128 //Render width of map
#define MAP_SIZE RENDER_DISTANCE * RENDER_DISTANCE // Size of map in x & z space

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
```

### Update Method Pattern
- Multiple object states are updated each frame based on the currently elapsed time (takes advantage of glfwGetTime())
- Ensures that existing animations (E.g. height changes to cloud Terrain, rotations/position changes to the UFO & Plane 3D models) are frame-independent, and smoother.

```cpp
// PLANE MODEL ANIMATIONS EXAMPLE
// Position/Rotation variables. Will determine how much the plane should move in the Y-Axis, and how much rotation should be applied in the X-Axis.
float bobOffset = sin((float)glfwGetTime() * bobSpeed) * bobAmount;
float tiltAngle = sin((float)glfwGetTime() * bobSpeed) * tiltAmount;

// Animations on the position/rotation of the plane overtime
model = translate(model, vec3(-3.5f, -1.4f + bobOffset, -9.f)); // World Position with vertical bobbing
model = rotate(model, radians(tiltAngle), vec3(1.0f, 0.0f, 0.0f)); // Pitching motion
```

## How the project mechanics work

### Window (GLFW)  & GLAD Initialization 
- On startup, the application starts a GLFW window with set dimensions (1280x720).
- First, GLFW is initialized with "glfwInit();". Once this is done, we call a function to lock and hide the cursor in the window, and finally we bind OpenGL context to the window.

```cpp
//Initialisation of GLFW
glfwInit();
//Initialisation of 'GLFWwindow' object
GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Peaceful Coexistence - CW2", NULL, NULL);
//Sets cursor to automatically bind to window & hides cursor pointer
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//Binds OpenGL to window
glfwMakeContextCurrent(window);
```

- After this is done, an attempt to initialize glad is made, returning an error if it fails to do so.

```cpp
//Initialisation of GLAD
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
    cout << "GLAD failed to initialise\n";
    return -1;
}
```

### Texture Loading (stb_image)
- Two textures are loaded for use in the Water model: A seamless water texture, and a brighter water texture:
- These textures use repeat wrapping and linear mipmap filtering properties in order to achieve a smooth appearance, regardless of distance.

```cpp
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
```

### Water Surface
- The Water Surface is a cube that has reduced height dimensions (so it looks flattened), and is position just below the Clouds terrain.
- The Vertices were defined first followed by the indices.
- Vertices define the position of the faces, the colors (Blue Shade/Gradient), and texture coordinates.
- Indices then define the 36 Triangles that form the six faces in the cube.

```cpp
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
```
- After this, the VAO, VBO and EBO are configured with the three vertex attribute in mind (As previously mention, these include the position, color and texture):

```cpp
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
```

- Once the vertices/indices are defined, and the VAO/VBO/EBO are buffered, the water needs to be drawn
- This is achieved in the render loop.
- First, the Water Shader is set, then we set the texture uniforms which will be passed in the waters fragment shader
- Then, we reset all transformations using mat4 back to default, and afterwards apply all necessary changes to the Water Model
- MVP Matrix is then calculated, and before drawing the water using indexed rendering, the textures are binded using 0 and 1 (Since there are two textures)

```cpp
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
waterModel = translate(waterModel, vec3(0.0f, -2.f, -1.5f));

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
```

### Procedurally-generated clouds.
- The Clouds model is a procedurally-generated terrain made possible using Perlins Noise from the FastNoiseLites dependency
- This provides the possibility to include Height variation and Cellular noise (For different colors within the generations)
- Two noise generaters were initialized for each, each configured with random seeds and a configured frequency

```cpp
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
```

- For height and color generation, the terrain vertices arrays store the position and color data (Each vertex has 6 floats).
- Perlin noise gives us values for Height, and colors are then assigned based on the value on the Cellular Noise (Colors include: White, Grey and Dark Grey)

```cpp
// Generate height and color data for each vertex
for (int y = 0; y < RENDER_DISTANCE; y++)
{
    for (int x = 0; x < RENDER_DISTANCE; x++)
    {
        // Setting of height from 2D noise value at respective x & y coordinate
        terrainVertices[i][1] = TerrainNoise.GetNoise((float)x, (float)y * 1.0f);

        // Retrieval of biome to set
        float biomeValue = BiomeNoise.GetNoise((float)x, (float)y);

        if (biomeValue <= -0.75f) // White Color
        {
            terrainVertices[i][3] = 1.0f;
            terrainVertices[i][4] = 1.0f;
            terrainVertices[i][5] = 1.0f;
        }
        else if (biomeValue <= -0.50f){ // Grey Color
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
```

- The Grid position is then calculated using a 0.0625-unit spacing value to form a grid in the X/Z coordinates in the scene

```cpp
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
```

- Finally, the last snippet in the generation code involves indice generation, these being responsible for forming two triangles per square in the grid in order to create the final mesh

```cpp
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
```

- Buffer Setup (Using VAO/VBO/EBO) is extremely similiar to how the water is configured, except it does not take take the texture attribute into account
- Moving to the Render Loop, in order to animate the terrain, the height values are regenerated each frame. This uses time (utilizing glfwGetTime()), and a timeScale float (which takes into account how fast it should change per frame)
- Once this is determined, then the Terrain (Clouds) VBO is updated

```cpp
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
```

- Finally, the Terrain is drawn into the scene using the same technique used with water.
- Transformations are reset using mat4 before any scaling/rotation/position is applied, the MVP matrix is calculated and finally the terrain is drawn using indexed rendering

```cpp
// Terrain (clouds) model matrix transformations
mat4 terrainModel = mat4(1.0f);
terrainModel = scale(terrainModel, vec3(2.0f, 2.0f, 2.0f));
terrainModel = rotate(terrainModel, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
terrainModel = translate(terrainModel, vec3(0.0f, -2.f, -1.5f));

// Calculates and sets MVP matrix for terrain (clouds)
mat4 terrainMVP = projection * view * terrainModel;
TerrainShaders.setMat4("mvpIn", terrainMVP);
TerrainShaders.setMat4("model", terrainModel); 

// Draws terrain using indexed rendering
glBindVertexArray(terrainVAO);
glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);
```

### Sun/Light Source
- The Light in the scene is rendered as a small white cube, positioned above everything else.
- sunVertices define the vertices for the cube

```cpp
float sunVertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    // Repeats 5 times after the first one.
```

- Similiar to the other models (Water/Clouds), the VAO/VBO are then created and buffered.
- This time, no EBO (ELement Buffer Object) is created, and two VAOs are created: sunVAO and lightVAO

```cpp
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
```

- In the render loop, the lighting position and color is defined, and afterwards passed into the shaders of all other models
- Example below shows these properties being passed to the Terrain Shader

```cpp
// !! LIGHT POSITION / COLOR !! 
vec3 lightPosition = vec3(0.0f, 5.0f, 0.0f);
vec3 lightCol = vec3(0.8f, 0.8f, 0.8f);

// !! DRAW TERRAIN !!
TerrainShaders.use();
TerrainShaders.setVec3("lightColor", lightCol);
TerrainShaders.setVec3("lightPos", lightPosition); 
TerrainShaders.setVec3("viewPos", cameraPosition);
```

- Finally, following the other model examples, its drawn into the scene.
- This time, however, Array Rendering is used rather than indice rendering.

```cpp
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
```

### UFO/Plane Models
- Using Assimp, the models are loaded before the render loop.
- Two formats were used: FBX (For UFO) and OBJ (For Plane)

```cpp
// 3D Model Loading
Model Ufo("media/ufo2/model/Low_poly_UFO.FBX"); // Ufo FBX
Model Plane("media/plane/floatplane/floatplane.obj"); // Plane OBJ
```

- In the render loop, before drawing the model we first call their appropriate shaders

```cpp
Shaders.use();
```

- The models are drawn similiar to other objects in the scene.
- The UFO is animated: It continously rotates in the Y-Axis. This is achieved by taking advantage of glfwGetTime(), as each time the loop runs its value updates, affecting the rotation value
- Afterwards, the UFO is drawn

```cpp
// Position/Rotation variables declared before main, as global variables
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
```

- The plane is also animated: Overtime, it bobs up and down overtime, and tilts to add in smoothness (to simulate flying)
- This is achieved by setting three global variables:

```cpp
// PLANE VARIABLES
float bobSpeed = 1.0f; // Controls how fast the plane bobs
float bobAmount = 0.3f; // Controls vertical movement distance
float tiltAmount = 2.0f; // Controls tilt angle in degrees
```

- Then, in the render loop we define a bobOffset and tiltAngle variables that determine how much the plane bobs (moves in the Y-Axis) and how much it tilts
- Both take into consideration the bobSpeed variable (Animation Speed)

```cpp
float bobOffset = sin((float)glfwGetTime() * bobSpeed) * bobAmount;
float tiltAngle = sin((float)glfwGetTime() * bobSpeed) * tiltAmount;
```

- Then, before drawing the model, these variables are applied to the models translate (position) and rotation.
- Translate takes the bobOffset so it can move the plane accordingly, whilst the rotation takes the tiltAngle into account.
- Since these values change each time the loop runs, the plane position/rotation is always updated as those variables value is always changing

```cpp
// Plane model matrix transformations
model = mat4(1.0f); //Model matrix
model = translate(model, planePosition + vec3(0.0f, bobOffset, 0.0f)); // World Position with vertical bobbing
model = rotate(model, radians(tiltAngle), vec3(1.0f, 0.0f, 0.0f)); // Pitching motion
model = scale(model, vec3(planeScale)); // Scaled down to appropriate size (1.0f is too large)

// Draws Plane after setting matrices 
SetMatrices(Shaders);
Shaders.setMat4("model", model);
Plane.Draw(Shaders);
```

### Shaders (Fragment/Vertex)
- Each object has their own respective fragment/vertex shader file.
- The Vertex Shaders are responsible for calculating Fragmeent Positiosn in the scene, alongside applying essential transfomrations to object vertices, sending colour coordinates to the fragment shaders and also the textures (Note: Not all of this apply to all vertex shaders, so for instance we don't pass any colors in the models vertex shaders so this is excluded).
- Example (From models vertex shader)

```cpp
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
```

- The Fragment Shaders, having received these information, are then going to process how lighting, textures and colors should be process in the scene.

```cpp
#version 460
//Colour value to send to next stage
out vec4 FragColor;

//Texture coordinates / Fragment position from last stage
in vec2 textureFrag;
in vec3 FragPos;

// Texture Sampler
uniform sampler2D texture_diffuse1;

// Lighting uniforms
uniform vec3 lightPos;
uniform vec3 lightColor;
```

- Each Fragment Shader (Excluding LightFragmentShader.frag) calculates how lighting is processed.
- This is done by calculating the normals, setting the ambient lighting, then adding diffuse lighting by computing the dot product between the normalized surface normal, and the vector for the light direction. Then finally, the final light output is the addition of the ambient lighting with the diffuse lighting

```
// Models Fragment Shader
 // normals calculation using derivatives
 vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));

 // Base Ambient Lighting
 float ambientStrength = 0.1;
 vec3 ambient = ambientStrength * lightColor;

 // Diffuse Lighting
 vec3 lightDir = normalize(lightPos - FragPos);
 float diff = max(dot(norm, lightDir), 0.0);
 vec3 diffuse = diff * lightColor;

 // Texture sampling
 vec4 texColor = texture(texture_diffuse1, textureFrag);
 
 //Setting of colour coordinates to colour map
 vec3 lighting = ambient + diffuse;
```

- The final output, which is outsourced to the program, is going to take into consideration the calculated lighting components along with other properties, such as Texture/Vertex RGB Colors, and the final variable determines the Alpha Channel, either set to 1 or using the Textures Alpha component (if available)

```cpp
// Terrain (Clouds) Fragment Shader example
vec3 result = (ambient + diffuse) * colourFrag;
FragColor = vec4(result, 1.0f); // Final output
```

## Exception handling 
- GLFW Window not initializing
```
//Checks if window has been successfully instantiated
if (window == NULL)
{
    cout << "GLFW Window did not instantiate\n";
    glfwTerminate();
    return -1;
}
```

- GLAD not initializing
```cpp
//Initialisation of GLAD
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
    cout << "GLAD failed to initialise\n";
    return -1;
}
```

- Texture failed to load
```cpp
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
```

## (Brief) Evaluation
- While the project is relatively simple, the biggest achievement for me was finding a way to animate all models (excluding the sun/water). This is especially true for the clouds, as I managed to use existing logic to my advantage in order to procedurally animate them.
- This is also true to animating the plane. I took advantage of the existing implementation with the UFOs rotation that use glfwGetTime() function and applied to to both its rotation and position (translate). 
- If I were to expand the project in the future, I would look into implement a bunch other features such as an animated light source, 3D sound sources to both ASSIMP Models, moving water (while keeping it a 3D Cube), and a skybox.
- For more advanced features, I wanted to look into animating the front of the planes front propeller but due to lack of knowledge I refrained from doing it for this current scope.

## Use of AI 
[AI Declaration Form](https://github.com/Tiago-Relvas-UoP/COMP3016-CW2/blob/main/documents/Student%20Declaration%20of%20AI%20Tool%20use%20in%20this%20Assessment.docx)

AI was used in the following capacity throughout the project:
- Code Review/Assistance:
- (1) Used as a Cleanup/Suggestions tool for existing code 
- (2) Used to find exact vertices coordinates to match water surface below clouds (Terrain) surface 
