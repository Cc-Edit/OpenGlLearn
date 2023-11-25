//
//  main.cpp
//  OpenGlDemo
//
//  Created by 文强 on 2023/11/25.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// 声明函数
// 按键事件，按下esc按钮时退出窗口
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
// 更新渲染视口
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// 全局变量
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
    
    // 初始化glad，是用来管理OpenGL的函数指针的
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 循环渲染，glfwWindowShouldClose获取窗口是否关闭
    while (!glfwWindowShouldClose(window))
    {
        // 输入检测
        processInput(window);

        // 渲染
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT); // 用上面设置的颜色清空屏幕

        glfwSwapBuffers(window); //函数会交换颜色缓冲, 将缓冲区内容绘制到屏幕
        glfwPollEvents(); // 检查有没有触发什么事件、更新窗口状态，并调用对应的回调函数
    }
    
    // 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
    glfwTerminate();
    return 0;
}

