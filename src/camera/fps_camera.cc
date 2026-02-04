#include <camera/fps_camera.hh>

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 fps_camera::get_view(void) const {
  glm::vec3 forward = look_get_forward();
  return glm::lookAt(position, position + forward, { 0.0f, 1.0f, 0.0f });
}

glm::mat4 fps_camera::get_projection(void) const {
  return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
}

void fps_camera::set_height(float height) {
  position.y = height;
}

void fps_camera::update(float dt) {
  glm::vec3 forward = move_get_forward();
  glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

  float velocity = speed * dt;

  if (input->keys[GLFW_KEY_W]) position += forward * velocity;
  if (input->keys[GLFW_KEY_S]) position -= forward * velocity;
  if (input->keys[GLFW_KEY_A]) position -= right * velocity;
  if (input->keys[GLFW_KEY_D]) position += right * velocity;

  double dx = input->xpos - last_x;
  double dy = last_y - input->ypos;

  last_x = input->xpos;
  last_y = input->ypos;

  yaw += dx * sensitivity;
  pitch += dy * sensitivity;

  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;
}
