#ifndef _input_hh_
#define _input_hh_

#include <iostream>

#include <GLFW/glfw3.h>

struct input_state {
  double xpos = 0.0;
  double ypos = 0.0;
  bool keys[1024] = { false };
  bool mouse_buttons[8] = { false };
};

class input_manager {
  public:
    input_state state;

    void init(GLFWwindow* window) {
      glfwSetWindowUserPointer(window, this);

      glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int state, int action, int mode) {
        static_cast<input_manager*>(glfwGetWindowUserPointer(window))->key_callback(window,
            key, state, action, mode);
      });
      
      glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        static_cast<input_manager*>(glfwGetWindowUserPointer(window))->mouse_button_callback(window,
            button, action, mods);
      });

      glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        static_cast<input_manager*>(glfwGetWindowUserPointer(window))->mouse_callback(window, x, y);
      });
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
      if (key >= 0 && key < 1024) {
        state.keys[key] = (action != GLFW_RELEASE);
      }
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
      if (button >= 0 && button < 8) {
        state.mouse_buttons[button] = (action != GLFW_RELEASE);
      }
    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
      state.xpos = xpos;
      state.ypos = ypos;
    }

};

#endif
