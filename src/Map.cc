#include <cstdint>
#include <fstream>
#include <omp.h>
#include <string>
#include <valarray>

extern uint64_t N_cell_x;
extern float L_cMpc;
extern std::string Output;

inline uint64_t Ind(uint64_t xx, uint64_t yy, uint64_t zz) {

  return zz + N_cell_x * (yy + N_cell_x * xx);
}

void Init_Map(std::valarray<float> &Map) {

  uint_fast64_t posx, posy, posz;

  #pragma omp parallel for num_threads(4) collapse(2) private(posx, posy, posz)
    for (posx = 0; posx < N_cell_x; ++posx)
      for (posy = 0; posy < N_cell_x; ++posy)
        for (posz = 0; posz < N_cell_x; ++posz)
          Map[Ind(posx, posy, posz)] = 0.0;
}

void Write_Map(std::valarray<float> &Map) {

  std::ofstream out{Output, std::ios_base::binary};

  char buffer[4096];

  out.rdbuf()->pubsetbuf(buffer, 4096);

  for (int i = 0; i < 3; ++i)
    out.write((char *)&N_cell_x, sizeof(uint64_t));

  float grid_space = L_cMpc / N_cell_x;

  out.write((char *)&grid_space, sizeof(float));

  out.write((char *)&Map[0], sizeof(float) * N_cell_x * N_cell_x * N_cell_x);

  out.close();
}
