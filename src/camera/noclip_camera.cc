#include <camera/noclip_camera.hh>

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 noclip_camera::get_view(void) const {
  glm::vec3 forward = get_forward();
  return glm::lookAt(position, position + forward, { 0.0f, 1.0f, 0.0f });
}

glm::mat4 noclip_camera::get_projection(void) const {
  return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
}

void noclip_camera::update(float dt) {
  glm::vec3 forward = get_forward();
  glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

  float velocity = speed * dt;

  if (input->keys[GLFW_KEY_W]) position += forward * velocity;
  if (input->keys[GLFW_KEY_S]) position -= forward * velocity;
  if (input->keys[GLFW_KEY_A]) position -= right * velocity;
  if (input->keys[GLFW_KEY_D]) position += right * velocity;

  if (input->mouse_buttons[GLFW_MOUSE_BUTTON_RIGHT]) {
    if (!show_cursor) {
      glfwSetInputMode(window->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      show_cursor = true;
    }

    double dx = input->xpos - last_x;
    double dy = last_y - input->ypos;
    last_x = input->xpos;
    last_y = input->ypos;

    yaw += dx * sensitivity;
    pitch += dy * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
  } else {
    if (show_cursor) {
      glfwSetInputMode(window->handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      show_cursor = false;
    }

    last_x = input->xpos;
    last_y = input->ypos;
  }
}

ray noclip_camera::create_mouse_ray(void) {
  float x = (2.0f * input->xpos) / window->width() - 1.0f;
  float y = 1.0f - (2.0f * input->ypos) / window->height();

  glm::vec4 clip(x, y, -1.0f, 1.0f);
  glm::vec4 eye = glm::inverse(get_projection()) * clip;
  eye = glm::vec4(eye.x, eye.y, -1.0f, 0.0f);

  glm::vec3 world = glm::normalize(glm::vec3(glm::inverse(get_view()) * eye));

  return { position, world };
}
