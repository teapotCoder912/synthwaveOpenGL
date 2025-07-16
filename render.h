#ifndef RENDER_H
#define RENDER_H
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<OpenGl/gl3.h>
#include<glm/gtc/type_ptr.hpp>
#include<vector>

class Window{
  GLFWwindow* window;
  int _width, _height;
  unsigned int VAO;
  unsigned int VBO;
  unsigned int shaderProgram;
  unsigned int lineVAO;
  unsigned int lineVBO;
  unsigned int lineShaderProgram;
  unsigned int sunVAO;
  unsigned int sunVBO;
  unsigned int sunShaderProgram;
  const float near = 0.1f;
  const float far = 30.0f;
  const float fov = 90.0f;
  float aspect;
  glm::mat4 projection;
  glm::mat4 model;
  glm::mat4 sunModel;

  glm::vec4 backgroundColor = glm::vec4(0.145f, 0.105f, 0.2901f, 1.0f);
  std::vector<float> vertices;
  std::vector<float> lineVertices;
  int n;
  int lengthK;

  bool changed = false;

  public:

  Window(int width, int height);
  ~Window();

  void Loop();

  void processInput();

  void setChange(bool flag) {changed = flag;}

  void surfaceTransform();

  friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);

};


#endif //RENDER_H
