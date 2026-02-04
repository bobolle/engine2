#ifndef _camera_hh_
#define _camera_hh_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class camera {
  public:
    camera(const glm::vec3& position, float yaw, float pitch, 
        float fov, float near_plane, float far_plane, float aspect) :
      position(position),
      yaw(-90.0f),
      pitch(0.0f),
      fov(70.0f),
      near_plane(0.1f), 
      far_plane(100.0f),
      aspect(aspect) {}
    virtual ~camera() {}

    glm::vec3 position;

    float yaw;
    float pitch;

    float fov;
    float near_plane;
    float far_plane;
    
    float aspect;

    virtual glm::mat4 get_view(void) const = 0;
    virtual glm::mat4 get_projection(void) const = 0;
    virtual void update(float dt) {}

    void set_aspect(float aspect) {
      this->aspect = aspect;
    }
};

#endif
