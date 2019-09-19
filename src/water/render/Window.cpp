#include "../../headers.h"

#include "Window.h"

//-------------------------------------------------------------------------------------------------
Window::Window()
{
    exiting_ = false;
    window_ = nullptr;
    position_ = glm::vec3(-240, 190, 650);
    viewMatrix_ = glm::lookAt(position_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    horizontalAngle_ = 2.77487f;
    verticalAngle_ = -0.35557f;

    speed_ = 200.0f; // units(meters) / second
    angleSpeed_ = 2*glm::pi<float>()/4;
}

//-------------------------------------------------------------------------------------------------
Window::~Window()
{
    glfwTerminate();

#ifdef FREEIMAGE_LIB
        FreeImage_DeInitialise();
#endif // FREEIMAGE_LIB
}

//-------------------------------------------------------------------------------------------------
void Window::init(int width, int height, const char* title)
{
    width_ = width;
    height_ = height;

#ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
#endif
    // Initialise GLFW
    if( !glfwInit() )
        throw std::runtime_error("Failed to initialize GLFW\n");

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window_ = glfwCreateWindow( width_, height_, title, NULL, NULL);
    if( window_ == nullptr )
    {
        glfwTerminate();
        throw std::runtime_error("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    }

    glfwMakeContextCurrent(window_);

    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, onResize);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);

    //
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window_, width_/2, height_/2);

    // projection
    updateProjection(width_, height_);
}

//-------------------------------------------------------------------------------------------------
void Window::updateProjection(int w, int h)
{
    width_ = w;
    height_ = h;
    projMatrix_ = glm::perspective(glm::radians(45.0f), ((float)width_)/height_, 5.0f, 10000.0f);
}

//-------------------------------------------------------------------------------------------------
void Window::clear()
{
    // Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Clear the screen buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//-------------------------------------------------------------------------------------------------
void Window::swapBuffers()
{
    glfwSwapBuffers(window_);
}

//-------------------------------------------------------------------------------------------------
void Window::processEvents()
{
    glfwPollEvents();

    exiting_ = glfwGetKey(window_, GLFW_KEY_ESCAPE ) == GLFW_PRESS || glfwWindowShouldClose(window_) != 0;

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle_) * sin(horizontalAngle_),
        sin(verticalAngle_),
        cos(verticalAngle_) * cos(horizontalAngle_)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle_ - 3.1415926f/2.0f),
        0.0f,
        cos(horizontalAngle_ - 3.1415926f/2.0f)
    );

    // Up vector
    glm::vec3 up = glm::cross( right, direction );

    // Move forward
    if (glfwGetKey( window_, GLFW_KEY_UP ) == GLFW_PRESS){
        position_ += direction * deltaTime * speed_;
    }
    // Move backward
    if (glfwGetKey( window_, GLFW_KEY_DOWN ) == GLFW_PRESS){
        position_ -= direction * deltaTime * speed_;
    }
    // Strafe right
    if (glfwGetKey( window_, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position_ += right * deltaTime * speed_;
    }
    // Strafe left
    if (glfwGetKey( window_, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position_ -= right * deltaTime * speed_;
    }

    // rotates
    if (glfwGetKey( window_, GLFW_KEY_D ) == GLFW_PRESS){
        horizontalAngle_ += angleSpeed_ * deltaTime;
    }
    if (glfwGetKey( window_, GLFW_KEY_A ) == GLFW_PRESS){
        horizontalAngle_ -= angleSpeed_ * deltaTime;
    }
    if (glfwGetKey( window_, GLFW_KEY_W ) == GLFW_PRESS){
        verticalAngle_ += angleSpeed_ * deltaTime;
    }
    if (glfwGetKey( window_, GLFW_KEY_S ) == GLFW_PRESS){
        verticalAngle_ -= angleSpeed_ * deltaTime;
    }

    // Camera matrix
    viewMatrix_       = glm::lookAt(
                                position_,           // Camera is here
                                position_+direction, // and looks here : at the same position, plus "direction"
                                up                  // Head is up (set to 0,-1,0 to look upside-down)
                           );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;

    transforms_.CamPos = position_;
    transforms_.HorizAngle = horizontalAngle_;
    transforms_.VertAngle = verticalAngle_;
    transforms_.Proj = projMatrix_;
    transforms_.View = viewMatrix_;
}

//-------------------------------------------------------------------------------------------------
void Window::onResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    Window* pw = (Window*)glfwGetWindowUserPointer(window);
    pw->updateProjection(width, height);
}
