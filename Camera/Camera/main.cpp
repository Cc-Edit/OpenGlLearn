//
//  main.cpp
//  Camera
//
//  Created by 文强 on 2023/11/28.
//

#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <iostream>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 全局变量
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera 方向
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f); // 摄像机位置
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 摄像机移动
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true; // 首次鼠标事件，记录初始换信息
float yaw   = -90.0f;    // 偏航角(Yaw)
float pitch =  0.0f; // 俯仰角(Pitch)
float lastX =  800.0f / 2.0; // 鼠标位置X
float lastY =  600.0f / 2.0; // 鼠标位置Y
float fov   =  45.0f; // 视角

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 声明函数
// 按键事件，按下esc按钮时退出窗口
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
// 更新渲染视口
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// 鼠标事件
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
// 滚动事件
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

// 顶点着色glsl
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";
// 片元着色
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "void main()\n"
    "{\n"
    "  FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.4);\n"
    "}\n\0";

int main()
{
    // glfw 初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 客户端版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// 客户端版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // APPLE 系统特殊处理
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // 创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 绑定窗口大小变化回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 绑定鼠标事件
    glfwSetCursorPosCallback(window, mouse_callback);
    // 绑定窗口滚动事件
    glfwSetScrollCallback(window, scroll_callback);
    
    // 通知window 捕获鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // 初始化glad，是用来管理OpenGL的函数指针的
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // 开启深度测试，遮挡z值较小的内容
    glEnable(GL_DEPTH_TEST);
    
    // 创建一个顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 加载顶点着色器代码
    glCompileShader(vertexShader); // 编译着色器
    // 检查着色器编译是否成功
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // 创建一个片元着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // 加载片元着色器代码
    glCompileShader(fragmentShader);// 编译着色器
    // 检查着色器编译是否成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    
    //关联着色器
    unsigned int shaderProgram = glCreateProgram(); // 创建一个着色器程序，着色器程序对象是多个着色器合并之后的版本
    glAttachShader(shaderProgram, vertexShader); // 挂载顶点着色器
    glAttachShader(shaderProgram, fragmentShader); // 挂载片元着色器
    glLinkProgram(shaderProgram); // 关联到opengl程序对象
    
    // 检查关联状态
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);  // 挂载后删除顶点着色器
    glDeleteShader(fragmentShader);// 挂载后删除片元着色器

    
    // 加载纹理图片
    int width, height, nrChannels;
    unsigned char *data = stbi_load("/Users/wenqiang/Documents/work/OpenGL/work/Camera/Camera/container.jpg", &width, &height, &nrChannels, 0);
    // 加载失败处理
    if (!data)
    {
        std::cout << "Failed to load texture1" << std::endl;
    }
    // 创建一个纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    // 定义纹理水平垂直防线的渲染方式，拉伸、重复、翻转
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // 纹理环绕方式，S方向repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // 纹理环绕方式，T方向repeat
    // 有时纹理不能完整覆盖模型，可以设置纹理缩小、放大时的过滤选项， GL_NEAREST 中心点采样，GL_LINEAR 临近采样
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载纹理图片数据data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // 为当前绑定的纹理自动生成所有需要的多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
    // 释放图片资源
    stbi_image_free(data);
    
    // 加载第二个纹理图片
    stbi_set_flip_vertically_on_load(true);  // 翻转第二张纹理
    data= stbi_load("/Users/wenqiang/Documents/work/OpenGL/work/Camera/Camera/awesomeface.png",  &width, &height, &nrChannels, 0);
    // 加载失败处理
    if (!data)
    {
        std::cout << "Failed to load texture2" << std::endl;
    }
    // 创建第二个纹理
    unsigned int texture_sec;
    glGenTextures(1, &texture_sec);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture_sec);
    // 定义纹理水平垂直防线的渲染方式，拉伸、重复、翻转
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // 纹理环绕方式，S方向repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // 纹理环绕方式，T方向repeat
    // 有时纹理不能完整覆盖模型，可以设置纹理缩小、放大时的过滤选项， GL_NEAREST 中心点采样，GL_LINEAR 临近采样
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载纹理图片数据data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // 为当前绑定的纹理自动生成所有需要的多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
    // 释放图片资源
    stbi_image_free(data);
    
    // 应用着色器程序使纹理生效
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
    
    // 3D立方体顶点
    float vertices[] = {
        //     ---- 位置 ----    - 纹理坐标,表示从纹理的哪个部分采样 -
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    // 多个立方体的位置坐标
    glm::vec3 cubePositions[] = {
      glm::vec3( 0.0f,  0.0f,  0.0f),
      glm::vec3( 2.0f,  5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3( 2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f,  3.0f, -7.5f),
      glm::vec3( 1.3f, -2.0f, -2.5f),
      glm::vec3( 1.5f,  2.0f, -2.5f),
      glm::vec3( 1.5f,  0.2f, -1.5f),
      glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // 创建一个顶点数组对象VAO
    glGenBuffers(1, &VBO); // 创建一个顶点缓冲对象VBO
    
    // 绑定顶点数组对象
    glBindVertexArray(VAO);
    // 绑定顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 将顶点数组复制到缓冲对象中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // 描述顶点结构：
    // 第一个参数：起始位置
    // 第二个参数：单个顶点元素个数，三维为3，四维为4
    // 第三个参数：顶点元素数据类型
    // 第四个参数：是否希望数据被标准化，设置为GL_TRUE，所有数据都会被映射到0到1之间
    // 第五个参数：步长 = 元素个数 * 元素类型长度
    // 第六个参数：位置数据在缓冲中起始位置的偏移量(Offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // 启用位置顶点属性，从 0 位置开始，对应着色器的 location = 0
    glEnableVertexAttribArray(0);
                          
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // 启用着色器颜色属性， 对应着色器的 location = 1
    glEnableVertexAttribArray(1);
    
    // 填充模式绘制
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // 循环渲染，glfwWindowShouldClose获取窗口是否关闭
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        
        //记录帧间距
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // 输入检测
        processInput(window);

        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 用上面设置的颜色清空屏幕， 同时清空深度缓存
        
        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_sec);
    
        // 使用挂载了着色器的程序对象
        glUseProgram(shaderProgram);
        
        // 创建模型矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // 创建一个观察矩阵，模拟摄像机
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); //定义一个投影矩阵
        // 查找uniform变量地址
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
        
        //绑定顶点数组
        glBindVertexArray(VAO);
        
        // 循环创建多个立方体
        for(unsigned int i = 0; i < 10; i++)
        {
          glm::mat4 model = glm::mat4(1.0f);
          model = glm::translate(model, cubePositions[i]);
          float angle = (float)currentFrame * i * 20.0;
          model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
          int modelLoc = glGetUniformLocation(shaderProgram, "model");
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // 不使用索引缓冲EBO,可以直接绘制顶点
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // 使用EBO的情况下,要使用glDrawElements来利用EBO绘制图形
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        glfwSwapBuffers(window); //函数会交换颜色缓冲, 将缓冲区内容绘制到屏幕
        glfwPollEvents(); // 检查有没有触发什么事件、更新窗口状态，并调用对应的回调函数
    }
    // 删除顶点数组
    glDeleteVertexArrays(1, &VAO);
    // 删除缓冲数组
    glDeleteBuffers(1, &VBO);
    // 删除程序对象
    glDeleteProgram(shaderProgram);
    // 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
    glfwTerminate();
    return 0;
}
