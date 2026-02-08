#ifndef _fps_camera_hh
#define _fps_camera_hh

#include <camera/camera.hh>
#include <common/units.hh>
#include <common/window.hh>
#include <input.hh>

class fps_camera : public camera {
  public:
    const input_state* input = nullptr;
    glfw_window* window;

    float speed;
    float sensitivity;

    bool show_cursor;

    fps_camera(glfw_window* window, const input_state* input, float aspect)
      : camera({0.0f, 0.0f, 0.0f}, 45.0f, 0.0f,
        70.0f, 0.1f, 400.0f, aspect),
        input(input), speed(10.0f), sensitivity(0.1f),
        show_cursor(true), window(window) {}

    glm::mat4 get_view(void) const override;
    glm::mat4 get_projection(void) const override;
    void update(float dt) override;
    void set_height(float height);

    glm::vec3 look_get_forward(void) const {
      glm::vec3 forward;
      forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      forward.y = sin(glm::radians(pitch));
      forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      return glm::normalize(forward);
    }

    glm::vec3 move_get_forward(void) const {
      glm::vec3 forward;
      forward.x = cos(glm::radians(yaw));
      forward.y = 0.0f;
      forward.z = sin(glm::radians(yaw));
      return glm::normalize(forward);
    }

  private:
    double last_x;
    double last_y;
};

#endif
