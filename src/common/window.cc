#include <common/window.hh>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

glfw_window::glfw_window(int width, int height, const std::string& title) : _width(width), _height(height) {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

  if (!_window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(_window);
  //glfwSetFramebufferSizeCallback(window, framebuffer_callback);

  glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

  glfwSetWindowUserPointer(_window, this);
  glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
  glViewport(0, 0, width, height);

  glEnable(GL_DEPTH_TEST);
  glfwSwapInterval(0);
}

glfw_window::~glfw_window(void) {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void glfw_window::clear(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void glfw_window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  auto* self = static_cast<glfw_window*>(glfwGetWindowUserPointer(window));
  self->_width = width;
  self->_height = height;

  glViewport(0, 0, width, height);

  if (self->_resize_callback) {
    self->_resize_callback(float(width) / float(height));
  }
}
