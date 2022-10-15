#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <iostream>
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    ShaderProgram shaderProgram("3.3_vertex_shader.vs", "3.3_fragment_shader.fs");


    // set up vertex data (and buffers(s)) and configure vertex attributes
    // shader������������ṩ����ϵ�, ����������ṩ��vertex�е�layout�й�
    // ͬʱ��Ҫ�޸�vertex attribute pointer(by glVertexAttribPointer api)
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };

    // vertices�൱��vertex shader func��input
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // ���õ�һ��������
    glBindVertexArray(VAO);

    // ���ƶ������鵽buffer�й�OpenGLʹ��
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ���ö�������Pointer
    // ����GPU�����η�������������ڴ�, ������3ά��, float��4�ֽ�, ���Բ�����12�ֽ�
    // ��һ��������vertex shader��position�й�
    // �ڶ������� => ����vec3, ������3
    // ���������� => ��������
    // ���ĸ����� => �Ƿ�ҪNormalize
    // ��������� => ����
    // const void * offset, �о��ȼ���uint64_t or uint32_t ��offset
    // ��֤��ʽ: ��vertices�����3��float,��ƫ����=12byte
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    // ���ö�������
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // ������������
    glEnableVertexAttribArray(1);

    // setup second Triangle
    // �⼸���൱��һ��������, �����ǰ󶨵�����VAOʱ, ��Ҫָ�������VBO
    // �����VAO
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // �߿�ģʽenable
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    auto start = std::chrono::high_resolution_clock::now();
    float offset = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // draw our first triangle
        // �����������֮ǰ�������ɫOrange
        shaderProgram.use();
        using namespace std::chrono_literals;
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        if (diff > 100ms) {
            start = now;
            offset += 0.1f;
            if (offset > 0.5f) {
                offset = 0.0f;
            }
            shaderProgram.set_uniform("myoffset", offset);
        }
        // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // ��VAOs[0] & ��Ⱦ
        glBindVertexArray(VAO);
        // �ڶ�������: �����������ʼ����, �൱��������offset?
        // ����������: һ�����ٸ�vertex
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}