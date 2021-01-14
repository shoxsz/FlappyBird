#ifndef _APP_
#define _APP_

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <functional>

class GLFWApp{
public:
  GLFWApp(const GLFWApp& app) = delete;
  GLFWApp(){}

  ~GLFWApp(){
    finish();
  }

  inline bool init(){
    return (initialized =  glfwInit());
  }

  inline void finish(){
    if(initialized){
      glfwTerminate();
    }
  }

  void setError(const std::string& error){
    this->error = error;
  }

  const std::string& getError() const{
    return error;
  }

private:
  bool initialized = false;
  std::string error;
};

class GameWindow{
public:
  friend class GLFWCallbacks;

  GameWindow(const GameWindow& gameWindow) = delete;

  GameWindow(GameWindow&& gameWindow){
    this->window = gameWindow.window;
    gameWindow.window = nullptr;
  }

  GameWindow(){}

  ~GameWindow(){
    if(window){
      glfwDestroyWindow(window);
      window = nullptr;
    }
  }

  bool create(float aspectRatio);

  inline void swapBuffers(){
    glfwSwapBuffers(window);
  }

  inline void pollEvents(){
    glfwPollEvents();
  }

  inline bool isAlive(){
    return !glfwWindowShouldClose(window);
  }

  GLFWwindow* window = nullptr;

private:
  void onResize(int width, int height);
  
  float aspectRatio;
};

#endif