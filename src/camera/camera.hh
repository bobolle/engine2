#ifndef _camera_hh_
#define _camera_hh_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class camera {
  public:
    virtual ~camera() {}
    virtual glm::mat4 get_view(void) const = 0;
    virtual glm::mat4 get_projection(void) const = 0;

    virtual void update(float dt) {}

    virtual void set_aspect(float aspect) {}
};

#endif
