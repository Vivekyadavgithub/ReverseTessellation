#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void processInput(GLFWwindow* window);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
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
            
            terrain.push_back(temp3);
            terrain.push_back(temp2);

            terrain.push_back(temp1);
            terrain.push_back(temp4);

            terrain.push_back(temp3);
            terrain.push_back(temp2);

            terrain.push_back(temp1);
            terrain.push_back(temp4);

            terrain.push_back(temp3);
            terrain.push_back(temp4);
        }
    }
    /*for (int i = 0; i < 6 * 2 * 100 * 100; i+= 6) {
        std::cout << terrain[i] << " " << terrain[i + 1] << " " << terrain[i + 2] << " "
            << terrain[i + 3] << " " << terrain[i + 4] << " " << terrain[i + 5] << "\n";
    }*/
    std::vector<float> triangle = {
         0.75, -0.75, 0.5, 1.0,
        -0.75, -0.75, 0.5, 1.0,
         0.75, 0.75, 0.5, 1.0
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 40 * 40 * 6 * sizeof(float), &terrain[0] , GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); 
    //glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(vao);

    const char* vertexShaderSource =
        R"GLSL(#version 410 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
       // "out vec3 vPosition;"
        void main()
        {
        const vec4 vertices[] = vec4[](vec4(0.75, -0.75, 0.5, 1.0),
            vec4(-0.75, -0.75, 0.5, 1.0),
            vec4(0.75, 0.75, 0.5, 1.0));
               //gl_Position = vertices[gl_VertexID];
        gl_Position =   projection * view * model  * vec4(aPos, 0.5, 1.0);
        })GLSL";

    const char* tesscontrolShaderSource =
        R"glsl(
        #version 410 core
        layout(vertices = 3) out;
    	//in vec3 vPosition[];
    	//out vec3 tcPosition[];
    	//uniform float TessLevelInner
    	//uniform float TessLevelOuter; 
        
        void main()
        {
        	//tcPosition[ID] = vPosition[ID];
            if (gl_InvocationID == 0) {
                gl_TessLevelInner[0] = 2.0;
                gl_TessLevelOuter[0] = 1.0;
                gl_TessLevelOuter[1] = 1.0;
                gl_TessLevelOuter[2] = 1.0;
            }
            gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
        }
        )glsl";

    const char* tessEvalShaderSource =
        R"glsl(
            #version 410 core
            layout(triangles, equal_spacing, cw) in;
            
            void main()
            {
                gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                              (gl_TessCoord.y * gl_in[1].gl_Position) +
                              (gl_TessCoord.z * gl_in[2].gl_Position);
            }
        )glsl";

    const char* fragmentShaderSource =
        R"glsl(
            #version 410 core
            out vec4 FragColor;
           
            void main()
            {
                

                FragColor = vec4(0.0, 1.0, 1.0, 1.0);
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
        
        //TessLevelInner
      //  unsigned int innerTess = glGetUniformLocation(shaderProgram, "TessLevelInner");
        //glUniform1f(innerTess, 4.0f);
        
        //unsigned int outerTess = glGetUniformLocation(shaderProgram, "TessLevelOuter");
        //glUniform1f(outerTess, 4.0f);
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        unsigned int view_loc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 model = glm::mat4(1.0f);
        unsigned int postions_loc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(postions_loc, 1, GL_FALSE, glm::value_ptr(model));
        
        glDrawArrays(GL_PATCHES, 0, 6 * 100 * 100);
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