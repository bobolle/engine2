#ifndef _noclip_camera_hh
#define _noclip_camera_hh

#include <camera/camera.hh>
#include <common/window.hh>
#include <input.hh>
#include <ray.hh>

class noclip_camera : public camera {
  public:
    glm::vec3 position;

    float yaw;
    float pitch;

    float fov;
    float aspect;
    float near_plane;
    float far_plane;

    const input_state* input = nullptr;
    glfw_window* window;

    float speed;
    float sensitivity;

    bool show_cursor;

    noclip_camera(glfw_window* window, const input_state* input, float aspect)
      : position(0.0f, 0.0f, 3.0f), yaw(-90), pitch(0.0f),
        fov(70), aspect(aspect),
        near_plane(0.1f), far_plane(100.0f),
        input(input), speed(10.0f), sensitivity(0.1f),
        show_cursor(true), window(window) {}

    glm::mat4 get_view(void) const override;
    glm::mat4 get_projection(void) const override;

    void update(float dt) override;
    void set_aspect(float aspect) override;

    ray create_mouse_ray(void);

  private:
    double last_x;
    double last_y;
};

#endif
