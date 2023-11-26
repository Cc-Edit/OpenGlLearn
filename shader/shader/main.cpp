//
//  main.cpp
//  shader
//
//  Created by 文强 on 2023/11/26.
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

// 顶点着色glsl
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "out vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos,1.0);\n"
    "   vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
    "}\0";
// 片元着色
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 vertexColor;\n" // 输入与顶点着色器的输出一致
    "void main()\n"
    "{\n"
    "   FragColor = vertexColor;\n"
    "}\n\0";

// 全局变量片元着色
const char *fragmentShaderSource1 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n" // 外部赋值
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
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
    
    // 初始化glad，是用来管理OpenGL的函数指针的
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    
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
    glShaderSource(fragmentShader, 1, &fragmentShaderSource1, NULL); // 加载片元着色器代码
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

    // 四个顶点
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // 右上
         0.5f, -0.5f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f,  // 左下
        -0.5f,  0.5f, 0.0f   // 左上
    };
    // 顶点组成目标图案的连接顺序
    unsigned int indices[] = {
        0, 1, 3,  // 013 连接形成一个三角形
        1, 2, 3   // 123 连接形成一个三角形
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // 创建一个顶点数组对象VAO
    glGenBuffers(1, &VBO); // 创建一个顶点缓冲对象VBO
    glGenBuffers(1, &EBO); // 创建一个元素缓冲对象EBO，也叫索引缓冲对象(Index Buffer Object，IBO)
    
    // 绑定顶点数组对象
    glBindVertexArray(VAO);
    // 绑定顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 将顶点数组复制到缓冲对象中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // 绑定元素缓冲对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // 将连接顺序复制到元素缓冲对象中
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 描述顶点结构：
    // 第一个参数：起始位置
    // 第二个参数：单个顶点元素个数，三维为3，四维为4
    // 第三个参数：顶点元素数据类型
    // 第四个参数：是否希望数据被标准化，设置为GL_TRUE，所有数据都会被映射到0到1之间
    // 第五个参数：步长 = 元素个数 * 元素类型长度
    // 第六个参数：位置数据在缓冲中起始位置的偏移量(Offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // 启用顶点属性，从 0 位置开始
    glEnableVertexAttribArray(0);

    // 清空缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 因为EBO还在使用，不能清空
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // 清空顶点缓冲
    glBindVertexArray(0);


    // 线框模式(Wireframe Mode) 绘制
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // 填充模式绘制
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // 循环渲染，glfwWindowShouldClose获取窗口是否关闭
    while (!glfwWindowShouldClose(window))
    {
        // 输入检测
        processInput(window);

        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT); // 用上面设置的颜色清空屏幕
        
        // 使用挂载了着色器的程序对象
        glUseProgram(shaderProgram);
        
        // 获取运行的秒数
        float timeValue = glfwGetTime();
        // 使用sin函数让颜色在0.0到1.0之间改变
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // 查询uniform ourColor的位置值
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // 设置uniform值， 注意更新之前要先使用glUseProgram使用该程序对象
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        
        //绑定顶点数组
        glBindVertexArray(VAO);
        // 不使用索引缓冲EBO,可以直接绘制顶点
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        // 使用EBO的情况下,要使用glDrawElements来利用EBO绘制图形
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        glfwSwapBuffers(window); //函数会交换颜色缓冲, 将缓冲区内容绘制到屏幕
        glfwPollEvents(); // 检查有没有触发什么事件、更新窗口状态，并调用对应的回调函数
    }
    // 删除顶点数组
    glDeleteVertexArrays(1, &VAO);
    // 删除缓冲数组
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // 删除程序对象
    glDeleteProgram(shaderProgram);
    // 当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
    glfwTerminate();
    return 0;
}
