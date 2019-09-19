#pragma once

class GLFWwindow;

#include "Transforms.h"

class Window
{
public:   
    Window();
    ~Window();

    void init(int width, int height, const char* title);
    bool isExiting() const {return exiting_;}
    GLFWwindow* getID() const {return window_;}

    void processEvents();

    void clear();
    void swapBuffers();

    const Transforms& getTransforms() const {return transforms_;}

protected:
    int width_;
    int height_;

    GLFWwindow* window_;
    bool exiting_;

    glm::mat4 viewMatrix_;
    glm::mat4 projMatrix_;

    // Initial position
    glm::vec3 position_;

    // Initial horizontal angle
    float horizontalAngle_;

    // Initial vertical angle
    float verticalAngle_;

    float speed_;
    float angleSpeed_;

    Transforms transforms_;

    void updateProjection(int w, int h);
    static void onResize(GLFWwindow* window, int width, int height);
};
