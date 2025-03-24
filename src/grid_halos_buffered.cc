#include <cmath>
#include <cstdint>
#include <omp.h>
#include <valarray>

extern uint64_t N_cell_x_orig;
extern uint64_t N_cell_x;

// Simply divides the halo positions by a factor N_original/N_new
void Coarse_Grid_Buffered(std::valarray<float> &Pos_x,
                          std::valarray<float> &Pos_y,
                          std::valarray<float> &Pos_z, uint32_t buff_sz) {

  uint_fast32_t i;

  float fac = 1.0 / (1. * N_cell_x_orig / N_cell_x);

  #pragma omp parallel num_threads(4)
  {

  #pragma omp for
    for (i = 0; i < buff_sz; ++i) {
      Pos_x[i] -= N_cell_x_orig * floorf(Pos_x[i] / N_cell_x_orig);
      Pos_x[i] *= fac;
    }

  #pragma omp for nowait
    for (i = 0; i < buff_sz; ++i) {
      Pos_y[i] -= N_cell_x_orig * floorf(Pos_y[i] / N_cell_x_orig);
      Pos_y[i] *= fac;
    }

  #pragma omp for nowait
    for (i = 0; i < buff_sz; ++i) {
      Pos_z[i] -= N_cell_x_orig * floorf(Pos_z[i] / N_cell_x_orig);
      Pos_z[i] *= fac;
    }
  }

} // End of Coarse_Grid()
