#include <cmath>
#include <cstdint>
#include <valarray>

extern uint64_t N_cell_x;
extern float L_cMpc;

inline uint64_t Ind(uint64_t xx, uint64_t yy, uint64_t zz) {

  return zz + N_cell_x * (yy + N_cell_x * xx);
}

void Cloud_in_Cell_Buffered(std::valarray<float> &Pos_x,
                            std::valarray<float> &Pos_y,
                            std::valarray<float> &Pos_z,
                            std::valarray<float> &Lum, uint32_t buff_sz,
                            std::valarray<float> &Field) {

  // For a given particle distribution,
  // this uses Cloud in Cell (CiC) to calculate
  // rho on the Grid.

  float Vol_unit = 1.0 / powf(L_cMpc / N_cell_x, 3.0);

  uint_fast8_t ii, jj, kk;
  uint_fast64_t i, j, k;
  uint_fast64_t ix, jy, kz;

  float wx, wy, wz;

  for (uint_fast64_t num = 0; num < buff_sz; ++num) {

    i = static_cast<int>(floorf(Pos_x[num]));
    j = static_cast<int>(floorf(Pos_y[num]));
    k = static_cast<int>(floorf(Pos_z[num]));

    for (ii = 0; ii <= 1; ++ii) {

      wx = 1.0 - fabs(Pos_x[num] - (i + ii));

      ix = (i + ii) % N_cell_x;

      for (jj = 0; jj <= 1; ++jj) {

        wy = 1.0 - fabs(Pos_y[num] - (j + jj));

        jy = (j + jj) % N_cell_x;

        for (kk = 0; kk <= 1; ++kk) {

          wz = 1.0 - fabs(Pos_z[num] - (k + kk));

          kz = (k + kk) % N_cell_x;

          Field[Ind(ix, jy, kz)] += wx * wy * wz * Vol_unit * Lum[num];
        }
      }
    }
  } // End of particle loop

} // End of Cloud_in_Cell()

void Nearest_grid_point_Buffered( std::valarray<float> &Pos_x,
                                  std::valarray<float> &Pos_y,
                                  std::valarray<float> &Pos_z,
                                  std::valarray<float> &Lum, uint32_t buff_sz,
                                  std::valarray<float> &Field)  {

  // For a given particle distribution, 
  // this maps the particle to the nearest grid point to calculate rho on the Grid.

  float Vol_unit = 1.0 / powf(L_cMpc / N_cell_x, 3.0);

  uint_fast8_t ii, jj, kk;
  uint_fast64_t i, j, k;
  uint_fast64_t ix, jy, kz;

  for (uint_fast64_t num = 0; num < buff_sz; ++num) {

    i = static_cast<int>(floorf(Pos_x[num]));
    j = static_cast<int>(floorf(Pos_y[num]));
    k = static_cast<int>(floorf(Pos_z[num]));

    if(Pos_x[num]-i > 0.5) ix = i+1;
	  else ix = i;
	  if(ix == N_cell_x) ix = 0;

    if(Pos_y[num]-j > 0.5) jy = j+1;
	  else jy = j;
	  if(jy == N_cell_x) jy = 0;

    if(Pos_z[num]-k > 0.5) kz = k+1;
	  else kz = k;
	  if(kz == N_cell_x) kz = 0;

    Field[Ind(ix, jy, kz)] += Vol_unit * Lum[num];
  } // End of particle loop

} // End of Nearest_grid_point()
