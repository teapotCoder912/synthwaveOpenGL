#include "render.h"
#include "perlin.h"
#include<iostream>



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Window *win =  (Window*)glfwGetWindowUserPointer(window);


    const float near = 0.1f;
    const float far = 30.0f;
    const float fov = 90.0f;
    const float aspect = static_cast<float>(width)/static_cast<float>(height);

    glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, near, far);

    glUseProgram(win->sunShaderProgram);
    GLint  sunProjectionUn= glGetUniformLocation(win->sunShaderProgram, "projection");
    std::cout << "sunProjectionUn: " << sunProjectionUn << std::endl;
    glUniformMatrix4fv(sunProjectionUn, 1, GL_FALSE, glm::value_ptr<float>(projection));

    glUseProgram(win->lineShaderProgram);
    GLint  lineProjectionUn= glGetUniformLocation(win->lineShaderProgram, "projection");
    std::cout << "lineProjectionUn: " << lineProjectionUn << std::endl;
    glUniformMatrix4fv(lineProjectionUn, 1, GL_FALSE, glm::value_ptr<float>(projection));

    glUseProgram(win->shaderProgram);
    GLint  ProjectionUn= glGetUniformLocation(win->shaderProgram, "projection");
    std::cout << "ProjectionUn: " << ProjectionUn << std::endl;
    glUniformMatrix4fv(ProjectionUn, 1, GL_FALSE, glm::value_ptr<float>(projection));
}

void Window::processInput()
{
    const float step = 0.01f;
    bool moved = false;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        model[3][1] += step;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        model[3][1] -= step;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        model[3][0] -= step;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        model[3][0] += step;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        model[3][2] -= step;
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        model[3][2] += step;
        moved = true;
    }

    if (moved) {
        setChange(true);
    }
}


Window::Window(int width, int height ) : _width(width), _height(height) {
    if (!glfwInit()) {
        std::cout << "Failes to initialize GLFW" << std::endl;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(_width, _height, "synthwave", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float verticesQuad[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f,
        0.5f, 0.0f, 0.0f,

        0.5f,  0.0f, 0.5f,
        0.5f,  0.0f, 0.0f,
        0.0f,  0.0f, 0.5f,
    };

    float verticesQuadLines[] ={
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f,

        0.0f, 0.0f, 0.5f,
        0.5f,  0.0f, 0.5f,

        0.5f,  0.0f, 0.5f,
        0.5f,  0.0f, 0.0f,

        0.5f,  0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
    };

    int linesVerticesNumber = sizeof(verticesQuadLines) / sizeof(float) / 3;
    int verticesNumber = sizeof(verticesQuad) / sizeof(float)/3;

    n = 50;
    lengthK = 10;

    float dx = 0, dz =0;
    for (int i = 0; i < n*lengthK; i++ ) {
        dx = 0;
        for (int j = 0; j < n;j++) {
            dx += 0.5f;
            for (int k = 0; k < verticesNumber; k++) {
                vertices.push_back(verticesQuad[k*3 + 0] + dx);
                vertices.push_back(verticesQuad[k*3 + 1]);
                vertices.push_back(verticesQuad[k*3 + 2] + dz);
            }

            for (int k = 0; k < linesVerticesNumber; k++) {
                lineVertices.push_back(verticesQuadLines[k*3 + 0] + dx);
                lineVertices.push_back(verticesQuadLines[k*3 + 1]);
                lineVertices.push_back(verticesQuadLines[k*3 + 2] + dz);
            }
        }
        dz -= 0.5f;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

    glBufferData(GL_ARRAY_BUFFER, lineVertices.size()*sizeof(float), lineVertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    int success;
    char Log[1000];

    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 projection;\n"
       "uniform mat4 model;\n"
        "void main()\n"
        "{\n"
        "float height = max(aPos.y, 1.25 + aPos.y/25.0f);"
        "   gl_Position = projection*model*vec4(aPos.x, height, aPos.z, 1.0);\n"
        "}\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1000, nullptr, Log);
        std::cout <<"Vertex::shader::error"<< Log << std::endl;
    }


    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\0";

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 1000, nullptr, Log);
        std::cout <<"Fragment::shader::error "<< Log << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 1000, nullptr, Log);
        std::cout <<"ShaderProgram::link::error "<< Log << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    const char* vertexLineShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "out float height;"
        "void main()\n"
        "{\n"
        "height = max(aPos.y, 1.25 + aPos.y/25.0f);"
        "gl_Position = projection*model*vec4(aPos.x, height+0.01f, aPos.z, 1.0);\n"
        "}\0";

    unsigned int vertexLineShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexLineShader, 1, &vertexLineShaderSource, NULL);
    glCompileShader(vertexLineShader);

    glGetShaderiv(vertexLineShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexLineShader, 1000, nullptr, Log);
        std::cout <<"Vertex::line::shader::error "<< Log << std::endl;
    }

const char* geometryLineShaderSource =
    "#version 330 core\n"
    "layout (lines) in;\n"
    "layout (triangle_strip, max_vertices = 4) out;\n"
    "float width = 0.0075f;\n"
    "in float height[];\n"
    "out float gHeight;\n"
    "void main()\n"
    "{\n"
    "if(gl_in[0].gl_Position.w <= 0.0f || gl_in[1].gl_Position.w <= 0.0f){\n"
    "return; }\n"
    "vec4 p0 = gl_in[0].gl_Position;\n"
    "vec4 p1 = gl_in[1].gl_Position;\n"
    "vec3 p0_ndc = p0.xyz/p0.w;\n"
    "vec3 p1_ndc = p1.xyz/p1.w;\n"
    "vec2 normal = normalize(p1_ndc.xy-p0_ndc.xy);\n"
    "vec2 perp = vec2(-normal.y, normal.x)*width/2.0f;\n"
    "vec3 v0 = p0_ndc + vec3(perp, 0.0f);"
    "vec3 v1 = p0_ndc - vec3(perp, 0.0f);"
    "vec3 v2 = p1_ndc + vec3(perp, 0.0f);"
    "vec3 v3 = p1_ndc - vec3(perp, 0.0f);"
    "gl_Position = vec4(v0,1.0f);\n"
    "gHeight = height[0];\n"
    "EmitVertex();\n"
    "gl_Position = vec4(v1,1.0f);\n"
    "gHeight = height[0];\n"
    "EmitVertex();\n"
    "gl_Position = vec4(v2,1.0f);\n"
    "gHeight = height[1];\n"
    "EmitVertex();\n"
    "gl_Position = vec4(v3,1.0f);\n"
    "gHeight = height[1];\n"
    "EmitVertex();\n"
    "EndPrimitive();\n"
    "}\0";


    unsigned int geometryLineShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryLineShader, 1, &geometryLineShaderSource, NULL);
    glCompileShader(geometryLineShader);

    glGetShaderiv(geometryLineShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometryLineShader, 1000, nullptr, Log);
        std::cout <<"Geometry::line::shader::error "<< Log << std::endl;
    }

    const char* fragmentLineShaderSource =
        "#version 330 core\n"
        "out vec4 color;\n"
        "in float gHeight;"
        "void main()\n"
        "{\n"
        "vec3 lowColor = vec3(0.866,0.137,0.7921);\n"
        "vec3 highColor = vec3(0,0,1);\n"
        "float h = clamp((gHeight - 0.0f)/10.0f,0.0f,1.0f);\n"
        "vec3 finColor = mix(lowColor,highColor,h);\n"
        "color = vec4(finColor, 1.0);\n"
        "}\0";

    unsigned int fragmentLineShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentLineShader, 1, &fragmentLineShaderSource, NULL);
    glCompileShader(fragmentLineShader);

    glGetShaderiv(fragmentLineShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentLineShader, 1000, nullptr, Log);
        std::cout <<"Fragment::shader::error "<< Log << std::endl;
    }

    lineShaderProgram = glCreateProgram();
    glAttachShader(lineShaderProgram, vertexLineShader);
    glAttachShader(lineShaderProgram, geometryLineShader);
    glAttachShader(lineShaderProgram, fragmentLineShader);

    glLinkProgram(lineShaderProgram);

    glGetProgramiv(lineShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(lineShaderProgram, 1000, nullptr, Log);
        std::cout <<"LineShaderProgram::link::shader::error "<< Log << std::endl;
    }

    glDeleteShader(vertexLineShader);
    glDeleteShader(geometryLineShader);
    glDeleteShader(fragmentLineShader);

    aspect =(float)width/(float)height;

    projection = glm::perspective(glm::radians(fov), aspect, near, far);

    model = glm::transpose(glm::mat4(
       1.0f, 0.0f, 0.0f, -((float)n)/4.0f,
       0.0f, 1.0f, 0.0f, -4.0f,
       0.0f, 0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 0.0f, 1.0f));


    glUseProgram(shaderProgram);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 1000, nullptr, Log);
        std::cout <<"ShaderProgram::Uniform::error "<< Log << std::endl;
    }

    glUseProgram(lineShaderProgram);
    modelLoc = glGetUniformLocation(lineShaderProgram, "model");
    projLoc = glGetUniformLocation(lineShaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glGetProgramiv(lineShaderProgram, GL_ACTIVE_UNIFORMS, &success);
    if (!success) {
        glGetProgramInfoLog(lineShaderProgram, 1000, nullptr, Log);
        std::cout <<"LineShaderProgram::Uniform::error "<< Log << std::endl;
    }

    // SUN PART
    float sunVertex[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f,
        0.5f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
        0.5f, 0.0f, 0.0f,
    };

    glGenVertexArrays(1, &sunVAO);
    glBindVertexArray(sunVAO);

    glGenBuffers(1, &sunVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunVertex), sunVertex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    const char* vertexSunShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "out vec2 v;\n"
        "void main(){\n"
        "gl_Position = projection * model*vec4(aPos,1.0f);\n"
        "v = aPos.xy;\n"
        "}\n";

    unsigned int vertexSunShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexSunShader, 1, &vertexSunShaderSource, NULL);
    glCompileShader(vertexSunShader);

    glGetShaderiv(vertexSunShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexSunShader, 1000, nullptr, Log);
        std::cout <<"VertexSunShader::error "<< Log << std::endl;
    }

    const char* fragmentSunShaderSource =
        "#version 330 core\n"
        "out vec4 color;\n"
        "in vec2 v;\n"
        "void main(){\n"
        "float radius = 0.2f;\n"
        "vec2 center = vec2(0.25f,0.25f);\n"
        "float len = length(center - v);\n"
        "if(len > radius || (v.y < 0.25f && sin(10.0f/v.y) <=0.0f)){\n"
        "discard;}\n"
        "vec3 lowColor = vec3(0.639216f,0.027451f,0.211765f);\n"
        "vec3 highColor = vec3(0.980392f,0.627451f,0.0196078f);\n"
        "float cof= smoothstep(0.1f,0.35f, v.y);\n"
        "vec3 resColor = mix(lowColor, highColor, cof);\n"
        "color = vec4(resColor, 1.0f);\n"
        "}\n";

    unsigned int fragmentSunShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentSunShader, 1, &fragmentSunShaderSource, NULL);
    glCompileShader(fragmentSunShader);

    glGetShaderiv(fragmentSunShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentSunShader, 1000, nullptr, Log);
        std::cout <<"FragmentSunShader::error "<< Log << std::endl;
    }

    sunShaderProgram = glCreateProgram();
    glAttachShader(sunShaderProgram, vertexSunShader);
    glAttachShader(sunShaderProgram, fragmentSunShader);

    glLinkProgram(sunShaderProgram);

    glGetProgramiv(sunShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(sunShaderProgram, 1000, nullptr, Log);
        std::cout <<"SunShader:link:::error "<< Log << std::endl;
    }

    glDeleteShader(vertexSunShader);
    glDeleteShader(fragmentSunShader);
    glUseProgram(sunShaderProgram);

    sunModel =  glm::transpose(glm::mat4(
    1.5f, 0.0f, 0.0f,-1.5*0.25f,
    0.0f, 1.5f, 0.0f,-0.3f,
    0.0f, 0.0f, 1.0f, -1.0f,
    0.0f, 0.0f, 0.0f, 1.0f
    ));

    GLint sunModelLoc = glGetUniformLocation(sunShaderProgram, "model");
    GLint sunProjectionLoc = glGetUniformLocation(sunShaderProgram, "projection");

    glUniformMatrix4fv(sunModelLoc, 1, GL_FALSE, glm::value_ptr(sunModel));
    glUniformMatrix4fv(sunProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


}

void Window::surfaceTransform() {

    for (int i =0; i < vertices.size(); i+=3) {
        float x = vertices[i+0];
        float y = vertices[i+1];
        float z = vertices[i+2];

        int len  = n*lengthK;

        if (z<= -len/4) {
            z+= len/4;
        }

        vertices[i+1] = perlin::fractalBrownianMotion2D(x,z) + pow(x - (float)n/4.0f,2)/20.0f;

    }

    for (int i =0; i < lineVertices.size(); i+=3) {
        float x = lineVertices[i+0];
        float y = lineVertices[i+1];
        float z = lineVertices[i+2];

        int len  = n*lengthK;

        if (z<= -len/4) {
            z+= len/4;
        }

        lineVertices[i+1] = perlin::fractalBrownianMotion2D(x,z) + pow(x - (float)n/4.0f,2)/20.0f;

    }

}

void Window::Loop() {

    GLint modShaderLoc = glGetUniformLocation(shaderProgram, "model");
    GLint modLineShaderLoc = glGetUniformLocation(lineShaderProgram, "model");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LESS);
    glEnable(GL_MULTISAMPLE);

    surfaceTransform();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(float), vertices.data());

    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, lineVertices.size()*sizeof(float), lineVertices.data());

    float speed = 0.05f;
    float lastTime = 0.0f;
    float currentTime = 0.0f;
    float tickElapsed = 0.0f;


    while (!glfwWindowShouldClose(window)) {

        processInput();

        tickElapsed = (currentTime - lastTime)*60.0f;
        setChange(tickElapsed);
        model[3][2] += speed*tickElapsed;
        if (model[3][2] > lengthK*n/4) {
            model[3][2] = 0.0f;
        }


        if (changed) {
            glUseProgram(lineShaderProgram);
            glUniformMatrix4fv(modLineShaderLoc, 1, GL_FALSE, glm::value_ptr(model));

            glUseProgram(shaderProgram);
            glUniformMatrix4fv(modShaderLoc, 1, GL_FALSE, glm::value_ptr(model));

            changed = false;
        }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);

        glBindVertexArray(sunVAO);
        glUseProgram(sunShaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);

        glBindVertexArray(lineVAO);
        glUseProgram(lineShaderProgram);
        glDrawArrays(GL_LINES, 0, lineVertices.size()/3);
        glDisable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();

        lastTime = currentTime;
        currentTime = glfwGetTime();

    }

}


Window::~Window() {

    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    glfwTerminate();

}

