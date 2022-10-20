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
    // shader和这里的数据提供是耦合的, 这里的数据提供和vertex中的layout有关
    // 同时需要修改vertex attribute pointer(by glVertexAttribPointer api)
    float vertices[] = {
        //---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   1.0f, 1.0f,// 右上
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   1.0f, 0.0f,// 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f,// 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,    0.0f, 1.0f// 左上
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // vertices相当于vertex shader func的input
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 绑定VAO&设置对应上下文(VBO/EBO)
    glBindVertexArray(VAO);

    // 复制顶点数组到buffer中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 设置顶点属性Pointer
    // 定义GPU如何如何访问我们输入的内存, 顶点是3维的, float是4字节, 所以步长是4 * 8字节
    // 第一个参数和vertex shader的position有关
    // 第二个参数 => 顶是vec3, 所以是3
    // 第三个参数 => 数据类型
    // 第四个参数 => 是否要Normalize
    // 第五个参数 => 步长
    // const void * offset, 感觉等价于uint64_t or uint32_t 的offset
    // 验证方式: 在vertices中添加3个float,即偏移量=12byte
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(0));
    // 启用顶点属性, 0表示layout position0
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
    // 启动顶点属性, 1表示layout position 1
    glEnableVertexAttribArray(1);

    // Texture是vec2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
    // 启动顶点属性, 1表示layout position 2
    glEnableVertexAttribArray(2);

    // Texture是vec2
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(8 * sizeof(float)));
    // 启动顶点属性, 1表示layout position 2
    glEnableVertexAttribArray(3);

    // setup second Triangle
    // 这几行相当于一个上下文, 当我们绑定到其他VAO时, 需要指定具体的VBO
    // 解除绑定VAO
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // 线框模式enable
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 创建纹理
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // 在绑定纹理之前先激活纹理单元, 纹理单元GL_TEXTURE0默认总是被激活, 所以只有一个纹理单元时不需要显示激活
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
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
        // 第一个参数: 纹理目标
        // 第二个参数: 为纹理指定多级渐远纹理的级别, 默认是0
        // 第三个参数: 纹理存储为何种格式, 这里采用RGB格式
        // 第四/第五个参数: 宽度/高度
        // 第六个参数: 0历史遗留问题
        // 第七第八个参数定义源图的格式和数据类型
        // 第九个参数是真正的图像数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    // 内存释放
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // 在绑定纹理之前先激活纹理单元, 纹理单元GL_TEXTURE0默认总是被激活, 所以只有一个纹理单元时不需要显示激活
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定的纹理对象设置环绕、过滤方式
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
    // 设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元
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

        // 渲染前激活&绑定texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        shaderProgram.use();
        shaderProgram.set_uniform("visibleParameter", visible);
        
        // 使用glm库进行计算
        glm::mat4 transform = glm::mat4(1.0f);
        
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));

        //unsigned int transformLoc = glGetUniformLocation(shaderProgram.getId(), "transform");
        //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        shaderProgram.set_uniform("transform", transform);

        // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // 绑定VAOs[0] & 渲染
        glBindVertexArray(VAO);
        // 第二个参数: 顶点数组的起始索引, 相当于有两个offset?
        // 第三个参数: 一共多少个vertex
        // indices的个数为6
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