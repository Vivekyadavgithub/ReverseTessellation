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
    std::vector<float> grid;

    for (float i = -100; i < 100; i+= 5) {
        for (float j = -100; j < 100; j+= 5) {
            float temp1 = i / 100;
            float temp2 = j / 100;
            float temp3 = temp1 + 0.05f;
            float temp4 = temp2 + 0.05f;

            grid.push_back(temp1);
            grid.push_back(temp2);

            grid.push_back(temp3);
            grid.push_back(temp2);

            grid.push_back(temp1);
            grid.push_back(temp4);

            grid.push_back(temp3);
            grid.push_back(temp2);

            grid.push_back(temp1);
            grid.push_back(temp4);

            grid.push_back(temp3);
            grid.push_back(temp4);
        }
    }
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 40 * 40 * 6 * sizeof(float), &grid[0] , GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

        uniform vec3 camera;

        in vec3 WorldPos_CS_in[];
        float r = 1.0f;
        out vec3 WorldPos_ES_in[];

        void main()
        {
           WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];


            gl_TessLevelOuter[0] =   distance(WorldPos_ES_in[0] , camera);
            gl_TessLevelOuter[1] =   distance(WorldPos_ES_in[0] , camera);

            gl_TessLevelOuter[2] =   distance(WorldPos_ES_in[0] , camera);

            gl_TessLevelInner[0] =   distance(WorldPos_ES_in[0] , camera);
       }
        )glsl";

    const char* tessEvalShaderSource =
        R"glsl(
            #version 410 core
            layout(triangles, equal_spacing, ccw) in;
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            in vec3 WorldPos_ES_in[];

            out vec3 WorldPos_FS_in;

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

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        unsigned int view_loc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        unsigned int model_loc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

        unsigned int cameraCoords = glGetUniformLocation(shaderProgram, "camera");
        glUniform3fv(cameraCoords, 1, glm::value_ptr(cameraPos));

        glDrawArrays(GL_PATCHES, 0, 4 * 40 * 100);
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
