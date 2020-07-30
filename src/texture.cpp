#include "texture.hpp"

#include "lodepng.h"
#include <iostream>

Texture2D* Texture2D::load(const std::string& filePath){
  Texture2D* texture = new Texture2D();
  std::vector<unsigned char> buffer;

  std::vector<unsigned char> pixels;
  unsigned int width, height;

  int result = lodepng::load_file(buffer, filePath);

  if(result){
    delete texture;
    return nullptr;
  }

  if(lodepng::decode(pixels, width, height, buffer)){
    delete texture;
    return nullptr;
  }

  texture->bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  texture->loadRGBAData(width, height, pixels.data());

  GLenum error = glGetError();
  if(error){
    delete texture;
    std::cout << error << std::endl;
    return nullptr;
  }

  return texture;
}