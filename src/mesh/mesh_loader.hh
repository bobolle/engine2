#ifndef _mesh_loader_hh_
#define _mesh_loader_hh_

#include <mesh/mesh.hh>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

mesh load_mesh_from_file(const std::string& path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cerr << "could not open file: " << path << std::endl;
  }

  std::vector<glm::vec3> positions;
  std::vector<vertex> vertices;
  std::vector<unsigned int> indices;

  std::string line;

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string prefix;
    ss >> prefix;

    if (prefix == "v") {
      glm::vec3 pos;
      ss >> pos.x >> pos.y >> pos.z;
      positions.push_back(pos);
    }
    else if (prefix == "f") {
      unsigned int i0, i1, i2;

      ss >> i0 >> i1 >> i2;

      indices.push_back(i0 - 1);
      indices.push_back(i1 - 1);
      indices.push_back(i2 - 1);
    }
  }

  vertices.reserve(positions.size());
  for (const auto& pos : positions) {
    vertex v;
    v.position = pos;
    v.color = glm::vec3(0.0f, 0.5f, 1.0f);
    vertices.push_back(v);
  }

  return mesh(vertices, indices);
}

#endif
