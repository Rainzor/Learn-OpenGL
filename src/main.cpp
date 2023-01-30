#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
// 顶点着色器源码
// 顶点着色器的输入的是顶点数据，顶点数据是在CPU中定义的，然后通过顶点缓冲对象（VBO）传递给顶点着色器的。
// 顶点着色器的输出是gl_Position，它是一个vec4类型的变量，它决定了顶点的最终位置。
const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// 片段着色器源码
const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // -----------------------------------------------------
    // glfw window creation 创建窗口
    // -----------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 在创建窗口之后，渲染循环初始化之前注册回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ---------------------------------------
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    // gladLoadGLLoader函数会在内部调用glfwGetProcAddress函数来获取相应的函数地址，
    // 然后把它们注册到OpenGL函数指针变量中，这样才能在渲染循环中调用它们。
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // -----------------------------------------------------
    // build and compile our shader program
    // -----------------------------------------------------
     
	// vertex shader 顶点着色器
	// 创建着色器对象(vertex shader object), 指定着色器类型为GL_VERTEX_SHADER
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// 把着色器源码附加到着色器对象上，并编译它
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors 编译检测
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    
	// fragment shader 片面着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    
	// link shaders 把两个着色器对象链接到一个用来渲染的着色器程序
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    // 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 绘制一个物体的时候都必须重复这一过程
    /* -----------------------------------------------------
        // 0. 复制顶点数组到缓冲中供OpenGL使用
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 1. 设置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 2. 当我们渲染一个物体时要使用着色器程序
        glUseProgram(shaderProgram);
        // 3. 绘制物体
        someOpenGLFunctionThatDrawsOurTriangle();
    ----------------------------------------------------- */

    //-------->使用VAO来节省不同VBO重复设置顶点属性的过程，特别是数据有不同location时<-------------
    /*
        // ..:: 初始化代码（只运行一次 (除非你的物体频繁改变)） :: ..
        // 1. 绑定VAO
        glBindVertexArray(VAO);
        // 2. 把顶点数组复制到缓冲中供OpenGL使用
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. 设置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        [...]

        // ..:: 绘制代码（渲染循环中） :: ..
        // 4. 绘制物体
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        someOpenGLFunctionThatDrawsOurTriangle();
    */

    // ------------------------------------------------------------------
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //VBO, EBO用于缓存顶点数据或者图元索引数据，从而避免每次绘制时的 CPU 与 GPU 之间的内存拷贝，可以改进渲染性能，降低内存带宽和功耗。 
    float vertices[] = {
        0.5f, 0.5f, 0.0f,    // top right
        0.5f, -0.5f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f, 0.5f, 0.0f    // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    // 声明缓冲对象ID 等价于 指针临时表示
    // glGen** 产生一个对象ID,不实际分配显存，
    // 第一个参数是GLsizei n 表示要产生的对象数量，
    // 第二个参数是GLuint *objects 表示产生的对象ID存放的地址
    unsigned int VBO, VAO, EBO;
    // VAO用于缓存顶点属性指针
    glGenVertexArrays(1, &VAO);
    // VBO用于缓存顶点数据
    glGenBuffers(1, &VBO);
    // EBO用于缓存图元索引数据，即以何种顺序绘制顶点
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // 绑定VAO，之后绑定和配置VBO和属性指针，最后解绑VAO供之后使用(实际不解绑也可以)
    glBindVertexArray(VAO);

    // 绑定对象至状态机的上下文(context)
    // 指定缓冲对象类型,将状态机中ARRAY_BUFFER状态与VBO绑定
    // -> 语法上相当于定义对象的类型 i.e. float* ptr;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 为缓冲对象分配GPU内存，将顶点数据复制到GPU缓冲的内存中
    // 由于之前的绑定，对于GL_ARRAY_BUFFER的所有操作都会影响当前绑定的缓冲对象
    //-> 语法上相当于指定对象的大小与内容 i.e. ptr = new float[100];
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);// 绑定EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// 为EBO分配内存


    //  为顶点属性指定数据格式，告诉OpenGL解析顶点数据的方式，以方便顶点着色器使用
    /*  每个顶点属性从一个VBO管理的内存中获得它的数据，而具体是从哪个VBO（程序中可以有多个VBO）获取
        则是通过在调用glVertexAttribPointer时绑定到GL_ARRAY_BUFFER的VBO决定的。由于在调用
        glVertexAttribPointer之前绑定的是先前定义的VBO对象，顶点属性0现在会链接到它的顶点数据。
    */
    // 参数1：顶点属性的位置值，顶点着色器中layout(location = 0)是position顶点属性的位置值(Location)
    // 参数2：顶点属性的大小，顶点属性是一个vec3，它由3个值组成，所以大小是3
    // 参数3：数据类型
    // 参数4：是否希望数据被标准化，如果我们希望数据在0.0（对于有符号型signed数据是-1.0）到1.0之间被标准化的话，我们可以传入GL_TRUE
    // 参数5：步长，连续的顶点属性组之间的间隔
    // 参数6：偏移量，该位置数据在缓冲buffer中起始位置的偏移量
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 启用顶点属性，顶点属性默认是禁用的
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    // 解除绑定，原对象的属性已经设置好，下次绑定时将复用该对象
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.（线绘制）
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // -----------------------------------------------------
    // render loop
    // -----------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        //先设置清空屏幕所用的颜色（状态设置函数）
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //再用glClear来清空屏幕的颜色缓冲（状态使用函数）
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        // 调用glUseProgram函数，用程序对象作为它的参数，以激活这个程序对象
        // 在glUseProgram函数调用之后，每个着色器调用和渲染调用都会使用这个程序对象
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);  // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        /*glDrawElements函数
            第一个参数指定了我们打算绘制的图元的类型，
            第二个参数指定了索引数组中的元素个数，
            第三个参数指定了索引的类型，
            最后一个参数是指定EBO中的偏移量
        */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        // glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
        glfwSwapBuffers(window);
        // glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
