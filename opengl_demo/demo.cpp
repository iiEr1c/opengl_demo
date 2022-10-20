#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float visible = 0.5f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ShaderProgram shaderProgram("4.1vertex_shader.vs", "4.1texture_shader.fs");


    // set up vertex data (and buffers(s)) and configure vertex attributes
    // shader������������ṩ����ϵ�, ����������ṩ��vertex�е�layout�й�
    // ͬʱ��Ҫ�޸�vertex attribute pointer(by glVertexAttribPointer api)
    float vertices[] = {
        //---- λ�� ----       ---- ��ɫ ----     - �������� -
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   1.0f, 1.0f,// ����
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   1.0f, 0.0f,// ����
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f,// ����
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,    0.0f, 1.0f// ����
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // vertices�൱��vertex shader func��input
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // ��VAO&���ö�Ӧ������(VBO/EBO)
    glBindVertexArray(VAO);

    // ���ƶ������鵽buffer�й�OpenGLʹ��
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ���ö�������Pointer
    // ����GPU�����η�������������ڴ�, ������3ά��, float��4�ֽ�, ���Բ�����4 * 8�ֽ�
    // ��һ��������vertex shader��position�й�
    // �ڶ������� => ����vec3, ������3
    // ���������� => ��������
    // ���ĸ����� => �Ƿ�ҪNormalize
    // ��������� => ����
    // const void * offset, �о��ȼ���uint64_t or uint32_t ��offset
    // ��֤��ʽ: ��vertices�����3��float,��ƫ����=12byte
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(0));
    // ���ö�������, 0��ʾlayout position0
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
    // ������������, 1��ʾlayout position 1
    glEnableVertexAttribArray(1);

    // Texture��vec2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
    // ������������, 1��ʾlayout position 2
    glEnableVertexAttribArray(2);

    // Texture��vec2
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(8 * sizeof(float)));
    // ������������, 1��ʾlayout position 2
    glEnableVertexAttribArray(3);

    // setup second Triangle
    // �⼸���൱��һ��������, �����ǰ󶨵�����VAOʱ, ��Ҫָ�������VBO
    // �����VAO
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // �߿�ģʽenable
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // ��������
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // �ڰ�����֮ǰ�ȼ�������Ԫ, ����ԪGL_TEXTURE0Ĭ�����Ǳ�����, ����ֻ��һ������Ԫʱ����Ҫ��ʾ����
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    // set the texture wrapping parameters
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        // ��һ������: ����Ŀ��
        // �ڶ�������: Ϊ����ָ���༶��Զ����ļ���, Ĭ����0
        // ����������: ����洢Ϊ���ָ�ʽ, �������RGB��ʽ
        // ����/���������: ���/�߶�
        // ����������: 0��ʷ��������
        // ���ߵڰ˸���������Դͼ�ĸ�ʽ����������
        // �ھŸ�������������ͼ������
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    // �ڴ��ͷ�
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // �ڰ�����֮ǰ�ȼ�������Ԫ, ����ԪGL_TEXTURE0Ĭ�����Ǳ�����, ����ֻ��һ������Ԫʱ����Ҫ��ʾ����
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    // set the texture wrapping parameters
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shaderProgram.use(); // don't forget to activate/use the shader before setting uniforms!
    // ����ÿ���������ķ�ʽ����OpenGLÿ����ɫ�������������ĸ�����Ԫ
    shaderProgram.set_uniform("texture1", 0);
    shaderProgram.set_uniform("texture2", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // draw our first triangle

        // ��Ⱦǰ����&��texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        shaderProgram.use();
        shaderProgram.set_uniform("visibleParameter", visible);
        
        // ʹ��glm����м���
        glm::mat4 transform = glm::mat4(1.0f);
        
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));

        //unsigned int transformLoc = glGetUniformLocation(shaderProgram.getId(), "transform");
        //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        shaderProgram.set_uniform("transform", transform);

        // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // ��VAOs[0] & ��Ⱦ
        glBindVertexArray(VAO);
        // �ڶ�������: �����������ʼ����, �൱��������offset?
        // ����������: һ�����ٸ�vertex
        // indices�ĸ���Ϊ6
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0); // no need to unbind it every time

        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
        float scaleAmount = static_cast<float>(abs(sin(glfwGetTime())));
        transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        shaderProgram.set_uniform("transform", transform);

        // draw again
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        visible += 0.0001f;
        if (visible >= 1.0f) {
            visible = 1.0f;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        visible -= 0.0001f;
        if (visible <= 0.0f) {
            visible = 0.0f;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}