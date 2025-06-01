#include <cmath>
#include <cstdint>
// #include <iostream>
#include <omp.h>
#include <valarray>

extern float L_cMpc;
extern uint64_t N_cell_x_orig;
extern uint64_t N_cell_x;
extern float z;
extern float h;
extern float Omega_m;

inline double Hubble (float zz) {
  return 100 * h * sqrt(Omega_m * powf(1.0 + zz, 3.0) + (1.0 - Omega_m));
}

// Applies redshift space distortions and divides the halo positions by a factor N_original/N_new
void Coarse_Grid_Buffered(std::valarray<float> &Pos_x,
                          std::valarray<float> &Pos_y,
                          std::valarray<float> &Pos_z,
                          std::valarray<float> &Vel_z,
                          uint32_t buff_sz,
                          uint32_t rsd_flag)  {

  uint_fast32_t i;

  float fac = 1.0 / (1. * N_cell_x_orig / N_cell_x);  // rescaling factor
  float gs = L_cMpc / N_cell_x_orig; // grid spacing

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
      if(rsd_flag == 1)
        Pos_z[i] += (1+z) * Vel_z[i] / (Hubble(z) * gs);  // redshift space distortion
      Pos_z[i] -= N_cell_x_orig * floorf(Pos_z[i] / N_cell_x_orig); // periodic boundary condition
      Pos_z[i] *= fac;
    }
  }

} // End of Coarse_Grid()
