#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <map>

#include "error.h"
#include "file.h"
#include "shader.h"

// 窗口尺寸
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAX_FRAGMENTS = 1024 * 1024 * 16; // 16M fragments

float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
};

float planeVertices[] = {
     5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.0f, 1.0f,
    -5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.0f, 1.0f,
    -5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.0f, 1.0f,
     5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 1.0f, 1.0f,
    -5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.0f, 1.0f,
     5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 1.0f, 1.0f,
};

float transparentVertices[] = {
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.5f,
     0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.5f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.5f,
};

float camX = 0.0f;
float camZ = 0.0f;

// A-Buffer 相关变量
GLuint headPointerTexture;
GLuint fragmentListBuffer;
GLuint atomicCounterBuffer;
GLuint opaqueShader;
GLuint transparentShader;
GLuint compositeShader;
GLuint quadVAO, quadVBO;

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camZ -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camZ += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camX -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camX += 0.01f;
}

// 初始化A-Buffer
void initABuffer() {
    // 1. 创建头指针纹理（每个像素存储链表头索引）
    glGenTextures(1, &headPointerTexture);
    glBindTexture(GL_TEXTURE_2D, headPointerTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, SCREEN_WIDTH, SCREEN_HEIGHT, 
                 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 2. 创建片段链表存储缓冲（SSBO）
    glGenBuffers(1, &fragmentListBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, fragmentListBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_FRAGMENTS * 24, NULL, GL_DYNAMIC_COPY);
    // 每个片段：vec4 color (16字节) + float depth (4字节) + uint next (4字节) = 24字节

    // 3. 创建原子计数器
    glGenBuffers(1, &atomicCounterBuffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

    // 4. 创建全屏四边形用于合成
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

// 清空A-Buffer
void clearABuffer() {
    // 重置头指针纹理（所有像素设为0xFFFFFFFF表示空链表）
    GLuint clearValue = 0xFFFFFFFF;
    glClearTexImage(headPointerTexture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &clearValue);
    
    // 重置原子计数器
    GLuint zero = 0;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounterBuffer);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
}

int main(int argc, char** argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          "A-Buffer Transparency", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gl3wInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallback, 0);
    glEnable(GL_DEPTH_TEST);

    // 编译着色器
    opaqueShader = CompileShader("opaque.vert", "opaque.frag");
    transparentShader = CompileShader("transparent.vert", "abuffer_build.frag");
    compositeShader = CompileShader("quad.vert", "abuffer_composite.frag");

    // 初始化A-Buffer
    initABuffer();

    // 设置VAO
    unsigned int VAO[3];
    glGenVertexArrays(3, VAO);
    unsigned int VBO[3];
    glGenBuffers(3, VBO);

    // cube
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // plane
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // transparent
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::vector<glm::vec3> positions;
    positions.push_back(glm::vec3(0, 0, 2.0f));
    positions.push_back(glm::vec3(0.5f, 0, 0.0f));
    positions.push_back(glm::vec3(0, 0, 1.0f));
    positions.push_back(glm::vec3(-0.5f, 0, 0.5f));
    positions.push_back(glm::vec3(0, 0, 1.5f));

    while (!glfwWindowShouldClose(window)) {
        static const GLfloat bgd[] = {0.02f, 0.5f, 0.75f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, bgd);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, -glm::vec3(camX, 0.0f, camZ));
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                                (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 
                                                0.001f, 10.0f);

        // ========== Pass 1: 渲染不透明物体 ==========
        glUseProgram(opaqueShader);
        glUniformMatrix4fv(glGetUniformLocation(opaqueShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(opaqueShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // cubes
        glBindVertexArray(VAO[0]);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        glUniformMatrix4fv(glGetUniformLocation(opaqueShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, -1.0f));
        model = glm::rotate(model, (float)glfwGetTime() / -2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        glUniformMatrix4fv(glGetUniformLocation(opaqueShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // floor
        glBindVertexArray(VAO[1]);
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(opaqueShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ========== Pass 2: 构建A-Buffer（收集透明片段）==========
        clearABuffer();
        
        glUseProgram(transparentShader);
        glUniformMatrix4fv(glGetUniformLocation(transparentShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(transparentShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        // 绑定A-Buffer资源
        glBindImageTexture(0, headPointerTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounterBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, fragmentListBuffer);
        
        // 禁用深度写入，但保持深度测试
        glDepthMask(GL_FALSE);
        glDisable(GL_BLEND);
        
        // 渲染透明三角形（不需要排序！）
        glBindVertexArray(VAO[2]);
        for (const auto& pos : positions) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            glUniformMatrix4fv(glGetUniformLocation(transparentShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        glDepthMask(GL_TRUE);

        // ========== Pass 3: 合成最终图像 ==========
        glUseProgram(compositeShader);
        glBindImageTexture(0, headPointerTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, fragmentListBuffer);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
        processKeyboard(window);
    }

    glDeleteVertexArrays(3, VAO);
    glDeleteBuffers(3, VBO);
    glDeleteTextures(1, &headPointerTexture);
    glDeleteBuffers(1, &fragmentListBuffer);
    glDeleteBuffers(1, &atomicCounterBuffer);

    glfwTerminate();
    return 0;
}