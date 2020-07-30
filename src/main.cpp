#include <list>
#include <ctime>
#include <iostream>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "app.hpp"
#include "texture.hpp"

#define CANVAS_ASPECT_RATIO (768.0f / 1024.0f)
#define CANVAS_HEIGHT (30)
#define CANVAS_WIDTH (CANVAS_HEIGHT * CANVAS_ASPECT_RATIO)

#define CANVAS_HALF_WIDTH (CANVAS_WIDTH/2)
#define CANVAS_HALF_HEIGHT (CANVAS_HEIGHT/2)

#define PIPE_GAP (CANVAS_HALF_WIDTH)
#define HOLE_GAP (6)
#define PIPE_WIDTH (4)

#define BASE_HEIGHT (4)
#define MIN_PIPE_HEIGHT (4)
#define GRAVITY (-9.8f * 2)
#define PLAYER_MASS (5.0f)

enum GameStates{
  PAUSED,
  PLAYING,
  DEAD
};

struct Hole{
  float x, y;
};

struct Player{
  float x, y;
  float speedX, speedY;
  float impulseY;
  float width, height;
  float animationStep;
  bool hit;
};

struct Quad{
  float left, right, bottom, top;
};

class Textures{
public:
  Textures(const Textures& texture) = delete;

  Textures(Textures&& texture){
    this->background = texture.background;
    texture.background = nullptr;
  }

  Textures(){}

  ~Textures(){
    if(background){
      delete background;
    }

    if(base){
      delete base;
    }

    if(pipe){
      delete pipe;
    }

    if(bird){
      delete bird;
    }
  }

  bool loadTextures(){
    background = Texture2D::load("assets/background-day.png");
    if(!background){
      return false;
    }

    base = Texture2D::load("assets/base.png");
    if(!base){
      return false;
    }
      
    pipe = Texture2D::load("assets/pipe-green.png");
    if(!pipe){
      return false;
    }

    bird = Texture2D::load("assets/yellow-bird.png");
    if(!bird){
      return false;
    }

    loaded = true;

    return true;
  }

  Texture2D* background;
  Texture2D* base;
  Texture2D* pipe;
  Texture2D* bird;
  bool loaded;
};

inline float getMax(float a, float b){
  return a > b ? a : b;
}

inline float getMin(float a, float b){
  return a < b ? a : b;
}

void updatePlayerPosition(Player& player, float deltasec){
	player.speedY += (GRAVITY * PLAYER_MASS + player.impulseY) * deltasec;

	if (player.speedY >= 5.0f) {
		player.impulseY = 0.0f;
	}

	player.x += player.speedX * deltasec;
	player.y += player.speedY * deltasec;

	if (player.y <= BASE_HEIGHT) {
		player.y = BASE_HEIGHT;
	}
}

void generateHoles(std::list<Hole>& holes, float playerX, int count){
  float lastHoleX = holes.size() > 0 ? holes.back().x : playerX + CANVAS_WIDTH;

  int maxHoleY = CANVAS_HEIGHT - MIN_PIPE_HEIGHT - (HOLE_GAP/2);
  int minHoleY = BASE_HEIGHT + MIN_PIPE_HEIGHT + (HOLE_GAP/2);

  for(unsigned int i = 0; i < count; ++i){
    float holeY = (rand() % (maxHoleY - minHoleY)) + minHoleY;
    float holeX = lastHoleX + ((i + 1) * CANVAS_HALF_WIDTH);

    holes.push_back({ holeX, holeY });
  }
}

void updateHoles(float playerX, std::list<Hole>& holes){
  if(holes.size() < 6){
    generateHoles(holes, playerX, 12 - holes.size());
  }

  while(!holes.empty()){
    const Hole& hole = holes.front();
    if(playerX - hole.x >= CANVAS_HALF_WIDTH + PIPE_WIDTH){
      holes.pop_front();
    }else{
      break;
    }
  }
}

void getHolePipes(const Hole& hole, Quad& top, Quad& bottom){
  float topPipeBottom = hole.y + (HOLE_GAP/2);
  float bottomPipeTop = hole.y - (HOLE_GAP/2);
  float halfWidth = PIPE_WIDTH / 2.0f;

  top = { hole.x - halfWidth, hole.x + halfWidth, topPipeBottom, topPipeBottom + 23 };
  bottom = { hole.x - halfWidth, hole.x + halfWidth, bottomPipeTop - 23, bottomPipeTop };
}

bool testColision(Player& player, Quad& quad){
  const float halfWidth = player.width / 2;
  const float halfHeight = player.height / 2;
  if(player.x + halfWidth < quad.left) return false;
  if(player.x - halfWidth > quad.right) return false;
  if(player.y + halfHeight < quad.bottom) return false;
  if(player.y - halfHeight > quad.top) return false;

  return true;
}

bool isPlayerHit(Player player, std::list<Hole>& holes){
  Quad topPipe, bottomPipe;

  std::list<Hole>::iterator it = holes.begin();
  while(it->x + PIPE_WIDTH / 2 < player.x){
    ++it;
    if(it == holes.end()){
      return false;
    } 
  }

  getHolePipes(*it, topPipe, bottomPipe);
  return testColision(player, topPipe) || testColision(player, bottomPipe);
  return false;
}

bool isPlayerDead(Player player){
  return player.y <= BASE_HEIGHT;
}

GLfloat* orthoProjection(float left, float right, float bottom, float top, float far, float near){
  GLfloat* matrix = new GLfloat[12];

  matrix[0] = 2 / (right - left);
  matrix[1] = 0;
  matrix[2] = 0;
  matrix[3] = -(right + left) / (right - left);

  matrix[4] = 0;
  matrix[5] = 2 / (top - bottom);
  matrix[6] = 0;
  matrix[7] = -(top + bottom) / (top - bottom);

  matrix[8] = 0;
  matrix[9] = 0;
  matrix[10] = -2 / (far - near);
  matrix[11] = -(far + near) / (far - near);

  matrix[12] = 0;
  matrix[13] = 0;
  matrix[14] = 0;
  matrix[15] = 1;

  return matrix;
}

void renderBackground(Textures& textures){
  textures.background->bind();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-CANVAS_HALF_WIDTH, -CANVAS_HALF_HEIGHT, 0.0f);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 1);
  glVertex2f(0, 0);
  
  glTexCoord2f(1, 1);
  glVertex2f(CANVAS_WIDTH, 0);

  glTexCoord2f(1, 0);
  glVertex2f(CANVAS_WIDTH, CANVAS_HEIGHT);
  
  glTexCoord2f(0, 0);
  glVertex2f(0, CANVAS_HEIGHT);
  glEnd();
}

void renderBase(Textures& textures){
  textures.base->bind();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-CANVAS_HALF_WIDTH, -CANVAS_HALF_HEIGHT, 0.0f);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 1);
  glVertex2f(0, 0);
  
  glTexCoord2f(1, 1);
  glVertex2f(CANVAS_WIDTH, 0);

  glTexCoord2f(1, 0);
  glVertex2f(CANVAS_WIDTH, BASE_HEIGHT);
  
  glTexCoord2f(0, 0);
  glVertex2f(0, BASE_HEIGHT);
  glEnd();
}

void renderBird(Player& player, Textures& textures, float deltasec){
  textures.bird->bind();

  float halfWidth = player.width / 2;
  float halfHeight = player.height / 2;

  float maxAngle = 30;
  float minAngle = -90;
  float maxSpeed = -20;
  float minSpeed = -30;

  float texX = 0;

  float clampedSpeed = getMax(minSpeed, getMin(maxSpeed, player.speedY));
  float angle = -90 + (clampedSpeed - minSpeed)*((maxAngle - minAngle)/(maxSpeed - minSpeed));
  
  player.animationStep += deltasec;
  if(angle > 0){
    texX = (int(player.animationStep * 12) % 3) / 3.0f;
  }

  glPushMatrix();
  glTranslatef(player.x, player.y, 0);
  glRotatef(angle, 0, 0, 1);
  glTranslatef(-player.x, -player.y, 0);

  glBegin(GL_QUADS);
  glTexCoord2f(texX, 1.0f);
  glVertex2f(player.x - halfWidth, player.y - halfHeight);

  glTexCoord2f(texX + 0.33333f, 1.0f);
  glVertex2f(player.x + halfWidth, player.y - halfHeight);

  glTexCoord2f(texX + 0.33333f, 0.0f);
  glVertex2f(player.x + halfWidth, player.y + halfHeight);

  glTexCoord2f(texX + 0.0f, 0.0f);
  glVertex2f(player.x - halfWidth, player.y + halfHeight);
  glEnd();

  glPopMatrix();
}

void renderPipes(const std::list<Hole>& holes, float playerX, Textures& textures){
  textures.pipe->bind();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-playerX, -CANVAS_HALF_HEIGHT, 0.0f);
  
  const unsigned int holesCount = holes.size();
  const float pipeDist = 8;
  unsigned int index = 0;
  glBegin(GL_QUADS);
  std::list<Hole>::const_iterator it = holes.begin();
  for(;it != holes.end(); ++it, ++index){
    const Hole& hole = *it;
    Quad topPipe, bottomPipe;

    getHolePipes(hole, topPipe, bottomPipe);

    //draw first pipe
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(topPipe.left, topPipe.bottom);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(topPipe.right, topPipe.bottom);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(topPipe.right, topPipe.top);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(topPipe.left, topPipe.top);
    
    //draw second pipe
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(topPipe.left, bottomPipe.bottom);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(topPipe.right, bottomPipe.bottom);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(topPipe.right, bottomPipe.top);
    
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(topPipe.left, bottomPipe.top);
  }
  glEnd();
}

int initGL(){
  if(!gladLoadGL()){
    return 0;
  }

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(){
  GLFWApp app;
  GameWindow gameWindow;
  Textures textures;
  GLfloat* projectionMatrix;
  std::list<Hole> holes;
  GameStates state = GameStates::PAUSED;

  Player player = { CANVAS_HALF_WIDTH, CANVAS_HALF_HEIGHT, 4.0f, 0.0f, 0.0f, 2.0f, 2.0f * 0.7f, 0.0f, false };
  float timer;

  bool pressed = false;

  srand(time(nullptr));

  if(!app.init()){
    return -1;
  }

  if(!gameWindow.create(CANVAS_ASPECT_RATIO)){
    return -1;
  }

  if(!initGL()){
    return -1;
  }

  if(!textures.loadTextures()){
    return -1;
  }

  projectionMatrix = orthoProjection(-CANVAS_HALF_WIDTH, CANVAS_HALF_WIDTH, -CANVAS_HALF_HEIGHT, CANVAS_HALF_HEIGHT, 1, -1);
  glClearColor(0, 0, 0, 1);
  timer = glfwGetTime();
  while(gameWindow.isAlive()){
    float currentTime = glfwGetTime();
    float delta = currentTime - timer;
    timer = currentTime;

    switch(state){
      case PAUSED:
      if(glfwGetMouseButton(gameWindow.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
        state = GameStates::PLAYING;
      }
      isPlayerHit(player, holes);
      break;
      case PLAYING:{
        if(glfwGetKey(gameWindow.window, GLFW_KEY_P) == GLFW_PRESS){
          state = GameStates::PAUSED;
        }

        int upState = glfwGetMouseButton(gameWindow.window, GLFW_MOUSE_BUTTON_1);
        if(upState == GLFW_PRESS){
          if(!pressed && !player.hit){
            player.speedY = getMax(0.0f, player.speedY);
            player.impulseY = -PLAYER_MASS * 15 * GRAVITY;
            pressed = true;
          }
        }else{
          pressed = false;
        }

        updatePlayerPosition(player, delta);
        updateHoles(player.x, holes);

        if(!player.hit){
          player.hit = isPlayerHit(player, holes);
        }

        if(isPlayerDead(player)){
          state = GameStates::DEAD;
        }
      }
      break;
      case DEAD:
      if(glfwGetMouseButton(gameWindow.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS){
        state = GameStates::PLAYING;
      }
      break;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix);

    renderBackground(textures);
    renderPipes(holes, player.x, textures);
    renderBird(player, textures, delta);
    renderBase(textures);

    gameWindow.swapBuffers();
    gameWindow.pollEvents();
  }

  delete projectionMatrix;

  return 0;
}