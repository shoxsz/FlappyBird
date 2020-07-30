#include "app.hpp"

class GLFWCallbacks{
public:
  static void framebufferResizeCallback(GLFWwindow* window, int w, int h){
    GameWindow* gameWindow = (GameWindow*)glfwGetWindowUserPointer(window);
    gameWindow->onResize(w, h);
  }
};

bool GameWindow::create(float aspectRatio){
  window = glfwCreateWindow(800, 640, "FlappyBird", nullptr, nullptr);
  
  if(!window){
    return false;
  }

  this->aspectRatio = aspectRatio;
  glViewport(0, 0, 640 * aspectRatio, 640);

  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, GLFWCallbacks::framebufferResizeCallback);
  
  return true;
}

void GameWindow::onResize(int width, int height){
  int adjustedWidth = height * aspectRatio;
  glViewport((width / 2) - (adjustedWidth / 2), 0, adjustedWidth, height);
}