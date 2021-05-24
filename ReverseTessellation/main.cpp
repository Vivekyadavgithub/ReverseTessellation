#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void processInput(GLFWwindow* window);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int main()
{
    GLFWwindow* window;
    if (!glfwInit()) return -1;

    window = glfwCreateWindow(1980, 1080, "Engine", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW ERROR\n";
    }
    /*int heightmapCols, heightRows, nrChannels;
    unsigned char* data = stbi_load("../snowterrain_heightmap.png", &heightmapCols, &heightRows, &nrChannels, 0);
    float* heightmap = new float[heightmapCols * heightRows];
    if (data) {
        for (int i = 0 ; i < heightRows ; i++) {
            for (int j = 0; j < heightmapCols; j++) {
                int index = j + i * heightmapCols;
                heightmap[index] = (data[index * nrChannels] / 255.0f);
            }
        }
    }*/
    std::vector<float> terrain;

    for (float i = -100; i < 100; i+= 5) {
        for (float j = -100; j < 100; j+= 5) {
            float temp1 = (float)i / 100;
            float temp2 = (float)j / 100;
            float temp3 = temp1 + 0.05f;
            float temp4 = temp2 + 0.05f;
           // std::cout << temp1 << " " << temp2 << " " << temp3 << " " << temp4 << "\n";
            terrain.push_back(temp1);
            terrain.push_back(temp2);
            //terrain.push_back(0.2f);

            terrain.push_back(temp3);
            terrain.push_back(temp2);
            //terrain.push_back(0.3f);

            terrain.push_back(temp1);
            terrain.push_back(temp4);
            //terrain.push_back(0.4f);

            terrain.push_back(temp3);
            terrain.push_back(temp2);
//            terrain.push_back(0.2f);

            terrain.push_back(temp1);
            terrain.push_back(temp4);
  //          terrain.push_back(0.8f);

            terrain.push_back(temp3);
            terrain.push_back(temp4);
    //        terrain.push_back(1.0f);

        }
    }
    /*for (int i = 0; i < 6 * 2 * 100 * 100; i+= 6) {
        std::cout << terrain[i] << " " << terrain[i + 1] << " " << terrain[i + 2] << " "
            << terrain[i + 3] << " " << terrain[i + 4] << " " << terrain[i + 5] << "\n";
    }*/
    float triangle[] = { 0.5f, 0.5f, -0.5f,
                         0.0f, 0.0f, -0.5f,
                         0.0f, 0.5f, -0.5f,
                         0.5f, 0.0f, -0.5f,
                         0.5f, -0.5f, -0.5f,
                         0.0f, -0.5f, -0.5f
    };
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 40 * 40 * 3  * sizeof(float), &terrain[0] , GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    /*
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(vao);

    GLint MaxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
    printf("Max supported patch vertices %d\n", MaxPatchVertices);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    const char* vertexShaderSource =
        R"GLSL(
        #version 410 core

        layout (location = 0) in vec2 Position_VS_in;

        out vec3 WorldPos_CS_in;

        void main()
        {
            WorldPos_CS_in = ( vec4(Position_VS_in, 0.0, 1.0)).xyz;
        }
        )GLSL";

    const char* tesscontrolShaderSource =
        R"glsl(
        #version 410 core
        layout (vertices = 3) out;

        uniform vec3 gEyeWorldPos;

        in vec3 WorldPos_CS_in[];

        out vec3 WorldPos_ES_in[];
        float GetTessLevel(float Distance0, float Distance1)
        {

            float AvgDistance = (Distance0 + Distance1) / 2.0;

            if (AvgDistance <= 2.0) {
                return 10.0;
            }
            else if (AvgDistance <= 5.0) {
                return 7.0;
            }
            else {
                return 3.0;
            }
        }
        void main()
        {
           vec3 geye;
           geye.x =  gEyeWorldPos.x * 100;
           geye.y = gEyeWorldPos.y * 100;
           geye.z = gEyeWorldPos.z;
           WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];

           float EyeToVertexDistance0 = distance(gEyeWorldPos, WorldPos_ES_in[0]);
           float EyeToVertexDistance1 = distance(gEyeWorldPos, WorldPos_ES_in[1]);
           float EyeToVertexDistance2 = distance(gEyeWorldPos, WorldPos_ES_in[2]);

           gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
           gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
           gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
           gl_TessLevelInner[0] = gl_TessLevelOuter[2];
       }
        )glsl";

    const char* tessEvalShaderSource =
        R"glsl(
            #version 410 core
            layout(triangles, equal_spacing, ccw) in;
            mat4 gVP = mat4(1.0f);
            float gDispFactor = 1.0f;
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            in vec3 WorldPos_ES_in[];

            out vec3 WorldPos_FS_in;
            vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
            {
                return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
            }

            vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
            {
                return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
            }
            void main()
            {
                WorldPos_FS_in = interpolate3D(WorldPos_ES_in[0], WorldPos_ES_in[1], WorldPos_ES_in[2]);
                gl_Position = projection * view * model  * vec4(WorldPos_FS_in, 1.0);
            }


        )glsl";

    const char* fragmentShaderSource =
        R"glsl(
            #version 410 core
            out vec4 FragColor;
            void main()
            {
                FragColor = vec4(1.0, 0.5, 0.5, 1.0);
            }
       )glsl";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);


    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int tessControlShader;
    tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tessControlShader, 1, &tesscontrolShaderSource, NULL);
    glCompileShader(tessControlShader);

    glGetShaderiv(tessControlShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tessControlShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::tesscontrol::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int tessEvalShader;
    tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tessEvalShader, 1, &tessEvalShaderSource, NULL);
    glCompileShader(tessEvalShader);


    glGetShaderiv(tessEvalShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tessEvalShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::TESSEVAL::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, tessControlShader);
    glAttachShader(shaderProgram, tessEvalShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(tessControlShader);
    glDeleteShader(tessEvalShader);
    glDeleteShader(fragmentShader);
    //float tfac = 0.2f;
    double xpos = 0, ypos = 0;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)700 / (float)700, 0.1f, 100.0f);
        unsigned int projection_loc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

        //TessLevelInner
        //unsigned int innerTess = glGetUniformLocation(shaderProgram, "TessLevelInner");
        //glUniform1f(innerTess, 4.0f);

        //unsigned int outerTess = glGetUniformLocation(shaderProgram, "TessLevelOuter");
        //glUniform1f(outerTess, 4.0f);
        glfwGetCursorPos(window, &xpos, &ypos);
        //std::cout << xpos << "  " << ypos << "\n";
        //unsigned int tessFactor = glGetUniformLocation(shaderProgram, "Tfactor");
        //glUniform1f(tessFactor, tfac);
        //tfac += 0.1f;
        //if (tfac > 3.0f) tfac = 0.2f;
        //std::cout << (float)xpos << " " << ypos << "\n";
        glm::vec3 pos = glm::vec3((float)xpos/100, (float)ypos/100, -0.5f);
        unsigned int cursorxpos = glGetUniformLocation(shaderProgram, "xpos");
        glUniform1f(cursorxpos, (float)xpos);
        unsigned int cursorypos = glGetUniformLocation(shaderProgram, "ypos");
        glUniform1f(cursorypos, (float)ypos);
        unsigned int gEyeWorld = glGetUniformLocation(shaderProgram, "gEyeWorldPos");
        glUniform3fv(gEyeWorld, 1, glm::value_ptr(pos));
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        unsigned int view_loc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        //glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.0f,0.0f, 1.0f));
        unsigned int postions_loc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(postions_loc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_PATCHES, 0, 2 * 4 * 20 * 100);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
