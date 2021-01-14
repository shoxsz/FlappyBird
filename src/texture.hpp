#ifndef _TEXTURE_
#define _TEXTURE_

#include <glfw/glfw3.h>
#include <string>

class Texture2D{
public:

  static Texture2D* load(const std::string& filePath);

  Texture2D(const Texture2D& texture) = delete;

  Texture2D(Texture2D&& texture){
    this->id = texture.id;
    this->width = texture.width;
    this->height = texture.height;
    texture.id = 0;
  }

  Texture2D(){
    glGenTextures(1, &id);
  }

  ~Texture2D(){
    if(id){
      glDeleteTextures(1, &id);
    }
  }

  inline void bind(){
    glBindTexture(GL_TEXTURE_2D, id);
  }

  inline void loadRGBAData(int width, int height, const GLubyte* pixels){
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    this->width = width;
    this->height = height;
  }

  int getWidth() const{
    return width;
  }

  int getHeight() const{
    return height;
  }

private:
  GLuint id;
  int width, height;
};

#endif