#ifndef _units_hh_
#define _units_hh_

namespace units {
  constexpr float metre = 1.0f;
  constexpr float tile_size = 4.0f * metre;

  constexpr int   tiles_per_chunk = 16;
  constexpr int   loaded_chunks_max = 5;

  constexpr float chunk_size = tiles_per_chunk * tile_size;
  constexpr float height_scale = 100.0f;
}

#endif
