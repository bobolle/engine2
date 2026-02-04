cmake_minimum_required(VERSION 3.16)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)

add_executable(game src/main.cc)
add_executable(builder tools/builder.cc)

set(GAME_FILES
  ${CMAKE_SOURCE_DIR}/src/common/window.cc
  ${CMAKE_SOURCE_DIR}/src/shader/shader.cc
  ${CMAKE_SOURCE_DIR}/src/camera/noclip_camera.cc
  ${CMAKE_SOURCE_DIR}/src/camera/fps_camera.cc
  ${CMAKE_SOURCE_DIR}/src/mesh/mesh_instance.cc
  ${CMAKE_SOURCE_DIR}/src/terrain/terrain.cc
)

add_library(game_lib OBJECT ${GAME_FILES})

target_include_directories(builder
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(game_lib
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(game_lib
  PUBLIC
    glad
)

target_link_libraries(builder
  PUBLIC
    glad
)

target_link_libraries(game
  PRIVATE
    game_lib
    glad
    OpenGL::GL
    glfw
)

if (glm_FOUND)
  target_include_directories(game PRIVATE ${GLM_INCLUDE_DIRS})
endif()
