#include <terrain/terrain.hh>
#include <common/units.hh>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <string>

std::vector<glm::vec3> load_tilemap(const std::string& path, int& width, int& height) {
  int n;
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &n, 3);
  if (!data) {
    std::cerr << "failed to load tilemap: " << path << "\n";
    exit(1);
  }

  std::vector<glm::vec3> colors(width * height);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int idx = (y * width + x) * 3;
      float r = data[idx + 0] / 255.0f;
      float g = data[idx + 1] / 255.0f;
      float b = data[idx + 2] / 255.0f;
      colors[y * width + x] = glm::vec3(r, g, b);
    }
  }

  stbi_image_free(data);
  return colors;
}

std::vector<float> load_heightmap(const std::string& path, int& width, int& height) {
  int n;
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &n, 1);
  if (!data) {
    std::cerr << "failed to load heightmap: " << path << "\n";
    exit(1);
  }

  std::vector<float> heights(width * height);
  for (int i = 0; i < width * height; ++i) {
    heights[i] = (float)data[i];
  }

  for (int i = 0; i < width * height; i = i + 16) {
    std::cout << heights[i] << "\n";
  }

  stbi_image_free(data);
  return heights;
}

chunk_disk create_chunk_from_data(glm::ivec2 chunk_coord, const std::vector<glm::vec3>& colors,
    const std::vector<float>& heights, int tw, int th, int hw, int hh) {
  chunk_disk chunk_tmp;
  chunk_tmp.coord = chunk_coord;

  int cx = chunk_coord.x;
  int cz = chunk_coord.y;

  for (int z = 0; z <= units::chunk_size; ++z) {
    for (int x = 0; x <= units::chunk_size; ++x) {
      int wx = cx * units::chunk_size + x;
      int wz = cz * units::chunk_size + z;

      wx = std::min(wx, hw - 1);
      wz = std::min(wz, hh - 1);

      chunk_tmp.heights[z][x] = heights[wz * hw + wx];
    }
  }

  for (int z = 0; z < units::chunk_size; ++z) {
    for (int x = 0; x < units::chunk_size; ++x) {
      int wx = cx * units::chunk_size + x;
      int wz = cz * units::chunk_size + z;

      wx = std::min(wx, tw - 1);
      wz = std::min(wz, th - 1);

      const glm::vec3& c = colors[wz * tw + wx];

      chunk_tmp.tiles[z][x].h00 = chunk_tmp.heights[z][x];
      chunk_tmp.tiles[z][x].h01 = chunk_tmp.heights[z][x + 1];
      chunk_tmp.tiles[z][x].h10 = chunk_tmp.heights[z + 1][x];
      chunk_tmp.tiles[z][x].h11 = chunk_tmp.heights[z + 1][x + 1];

      chunk_tmp.tiles[z][x].type = none;
      chunk_tmp.tiles[z][x].color[0] = c.r;
      chunk_tmp.tiles[z][x].color[1] = c.g;
      chunk_tmp.tiles[z][x].color[2] = c.b;
    }
  }

  return chunk_tmp;
}

int main(int argc, char** argv) {
  // usage: ./builder <tilemap.png> <heightmap.png> <output_path>
  std::string tilemap_path = argv[1];
  std::string heightmap_path = argv[2];
  std::string output_path = argv[3];

  int tw, th, hw, hh;
  auto colors = load_tilemap(tilemap_path, tw, th);
  auto heights = load_heightmap(heightmap_path, hw, hh);

  if (hw != tw + 1 || hh != th + 1) {
    std::cerr << "Heightmap must be (tilemap + 1) in each dimension\n";
    return 1;
  }

  std::cout << "tilemap size: " << tw << "x" << th << "\n";
  std::cout << "heightmap size: " << hw << "x" << hh << "\n";

  int chunks_x = tw / units::chunk_size;
  int chunks_z = th / units::chunk_size;

  for (int cz = 0; cz < chunks_z; ++cz) {
    for (int cx = 0; cx < chunks_x; ++cx) {
      chunk_disk chunk_tmp = create_chunk_from_data({cx, cz}, colors, heights, tw, th, hw, hh);
      std::string filename = output_path + "/chunk_"
        + std::to_string(cx) + "_" + std::to_string(cz) + ".bin";

      std::ofstream out(filename, std::ios::binary);
      if (!out) {
        std::cerr << "failed to open file for writing: " << filename << "\n";
        return 1;
      }

      out.write((char*)&chunk_tmp, sizeof(chunk_disk));
      out.close();

      std::cout << "saved: " << filename << "\n";
    }
  }

  return 0;
}
