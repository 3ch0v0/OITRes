#pragma once
#include <string>
#include <GLFW/glfw3.h> 


inline void showFPS(GLFWwindow* window, std::string windowName = " ")
{
    static double lastTime = glfwGetTime();
    static int nbFrames = 0;

    double currentTime = glfwGetTime();
    nbFrames++;

    if (currentTime - lastTime >= 1.0)
    {
        double msPerFrame = 1000.0 / double(nbFrames);

        std::string title = windowName + " - ["
            + std::to_string(nbFrames) + " FPS] ["
            + std::to_string(msPerFrame) + " ms]";

        glfwSetWindowTitle(window, title.c_str());

        nbFrames = 0;
        lastTime += 1.0;
    }
}
