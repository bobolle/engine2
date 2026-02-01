#ifndef _window_hh_
#define _window_hh_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

class glfw_window {
  public:
    glfw_window(int width, int height, const std::string& title);
    ~glfw_window();

    int width(void) const { return _width; }
    int height(void) const { return _height; }
    float aspect(void) const { return float(_width) / float(_height); }

    void clear(void);

    GLFWwindow* handle() const { return _window; }

    using resize_callback_t = std::function<void(float)>;
    void set_resize_callback(resize_callback_t cb) {
      _resize_callback = cb;
    }

  private:
    GLFWwindow* _window;
    int _width;
    int _height;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    resize_callback_t _resize_callback;
};

#endif
