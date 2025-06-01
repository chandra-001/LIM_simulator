#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string>
#include <valarray>

#include "cic_buffered.hh"
#include "grid_halo.hh"
#include "lum_buffered.hh"
#include "map.hh"
#include "read_halo.hh"

extern std::string Dir;
extern std::string Output;
extern float z; // redshift
extern float scaling_unit;  // scaling unit for the output intensity maps
extern float h; // Hubble constant in units of 100 km/s/Mpc
extern float Omega_m; // matter density parameter
extern double Lambda; // wavelength of the line in metres

extern uint64_t N_cell_x_orig;  // original number of cells in the grid
extern uint64_t N_cell_x; // number of cells in the grid after coarse-gridding

extern uint32_t Buf_sz; // buffer size for reading halos
extern uint32_t rsd_flag;  // redshift space distortion flag

inline double Hubble (float zz) {

  return 100 * h * sqrt(Omega_m * powf(1.0 + zz, 3.0) + (1.0 - Omega_m));
}

inline uint64_t Ind (uint64_t xx, uint64_t yy, uint64_t zz) {
  return zz + N_cell_x * (yy + N_cell_x * xx);
}

void Intensity (std::valarray<float> &);
void CO_Brightness_Temp (std::valarray<float> &);
void HI_Brightness_Temp_Wolz (std::valarray<float> &);
void HI_Brightness_Temp_Navarro (std::valarray<float> &);

void LIM_Buffered()
{
  std::ifstream num_halo {Dir + "/num_halos.txt"};  // file pointer for num_halos.txt

  uint64_t N_halo;  // stores the number of halos
  num_halo >> N_halo;
  num_halo.close();

  uint32_t loop = N_halo / Buf_sz;  // no. of times to read halos
  uint32_t rem = N_halo % Buf_sz; // no. of halos remaining

  std::valarray<float> Pos_x(Buf_sz), Pos_y(Buf_sz), Pos_z(Buf_sz);
  std::valarray<float> Vel_z(Buf_sz), Lum(Buf_sz), Zgas(Buf_sz);

  std::array<char *, 6> buff{
      reinterpret_cast<char *>(&Pos_x[0]), reinterpret_cast<char *>(&Pos_y[0]),
      reinterpret_cast<char *>(&Pos_z[0]), reinterpret_cast<char *>(&Vel_z[0]),
      reinterpret_cast<char *>(&Lum[0]), reinterpret_cast<char *>(&Zgas[0])};

  std::valarray<float> Map(N_cell_x * N_cell_x * N_cell_x);

  Init_Map(Map);  // initialises voxels to 0

  std::array<std::ifstream, 6> in;  // array of file pointers for the halo files

  File_Handler(in, 0);  // opens the halo files mentioned in the function; flag=0 --> open

  for (uint32_t i = 0; i < loop; ++i) {
    Read_Halos_Buffered(in, buff, Buf_sz);
    Coarse_Grid_Buffered(Pos_x, Pos_y, Pos_z, Vel_z, Buf_sz, rsd_flag);
    // Cii_Lum_Buffered_R22(Lum, Buf_sz); // assigns L_CII [L_sun units] to halos
    // Cii_Lum_Buffered_L18(Lum, Zgas, Buf_sz);  // assigns L_CII [L_sun units] to halos; accepts halos with nonzero metallicities only
    // CO_Lum_Buffered_Li16(Lum, Buf_sz);
    // CO_Lum_Buffered_Li16K20(Lum, Buf_sz);
    // CO_Lum_Buffered_Y22(Lum, Buf_sz);
    HI_mass_Buffered(Lum, Buf_sz);  // assigns HI mass [M_sun units] to halos
    // Cloud_in_Cell_Buffered(Pos_x, Pos_y, Pos_z, Lum, Buf_sz, Map);  // course grids to a density field (M_sun/Mpc^3) using CIC
    Nearest_grid_point_Buffered(Pos_x, Pos_y, Pos_z, Lum, Buf_sz, Map);  // course grids to a density field (M_sun/Mpc^3) using NGP
  }

  Read_Halos_Buffered(in, buff, rem);
  Coarse_Grid_Buffered(Pos_x, Pos_y, Pos_z, Vel_z, rem, rsd_flag);
  // Cii_Lum_Buffered_R22(Lum, rem);
  // Cii_Lum_Buffered_L18(Lum, Zgas, rem);
  // CO_Lum_Buffered_Li16(Lum, rem);
  // CO_Lum_Buffered_Li16K20(Lum, rem);
  // CO_Lum_Buffered_Y22(Lum, rem);
  HI_mass_Buffered(Lum, rem);
  // Cloud_in_Cell_Buffered(Pos_x, Pos_y, Pos_z, Lum, rem, Map);
  Nearest_grid_point_Buffered(Pos_x, Pos_y, Pos_z, Lum, rem, Map);

  File_Handler(in, 1); // closes the halo files

  // Intensity(Map);  // for CII maps
  // CO_Brightness_Temp(Map); // for CO maps
  // HI_Brightness_Temp_Wolz(Map); // for 21-cm maps
  HI_Brightness_Temp_Navarro(Map);  // for 21-cm maps

  Write_Map(Map);

} // End of LIM_Buffered()

void Intensity(std::valarray<float> &Mapp)
{
  const float solar_lum = 3.828e26; // in units of W
  const double mpc3_to_m3 = pow(3.086e22, 3.0); // Mpc^3 to m^3 conversion
  const double lum_den_fac = solar_lum / mpc3_to_m3;  // power of solar radiation in a 1Mpc^3 volume in units of W/m^3
  const float jansky = 1e-26; // in units of W
  const float Hubble_fac = 3.24e-20;  // conversion factor from km/s/Mpc to s^-1

  float fac = lum_den_fac * Lambda / (4 * M_PI * Hubble_fac * Hubble(z)) / jansky;
  uint_fast64_t ii, jj, kk;

  #pragma omp parallel for num_threads(4) collapse(2) private (ii, jj, kk)
    for (ii = 0; ii < N_cell_x; ++ii)
      for (jj = 0; jj < N_cell_x; ++jj)
        for (kk = 0; kk < N_cell_x; ++kk)
          Mapp[Ind(ii, jj, kk)] *= fac * scaling_unit;

} // End of Intensity()

void CO_Brightness_Temp (std::valarray<float> &Mapp)
{
  const float solar_lum = 3.828e26; // in units of W
  const double mpc3_to_m3 = pow (3.086e22, 3.0);
  const double lum_den_fac = solar_lum / mpc3_to_m3;
  const float k_B = 1.38e-23;
  const float Hubble_fac = 3.24e-20;

  const float fac =
      lum_den_fac * pow (Lambda, 3.0) * powf (1.0 + z, 2.0) /
      (8.0 * M_PI * k_B * Hubble_fac * Hubble(z)); // Convert to Kelvin

  uint_fast64_t ii, jj, kk;

  #pragma omp parallel for num_threads(4) collapse(2) private (ii, jj, kk)
    for (ii = 0; ii < N_cell_x; ++ii)
      for (jj = 0; jj < N_cell_x; ++jj)
        for (kk = 0; kk < N_cell_x; ++kk)
          Mapp[Ind(ii, jj, kk)] *= fac * scaling_unit;

} // End of Brightness_Temp()

void HI_Brightness_Temp_Wolz(std::valarray<float> &Mapp)
// returns value in kelvin; mismatch in power spectrum from literature
{
  const float h_P = 6.63e-34;  // Planck constant
  const float c = 3.0e8;  // speed of light in vacuum
  const float A_21 = 2.87e-15;  // Einstein spontaneous emission coefficient
  const float m_H = 1.67e-27;  // mass of H-atom
  const float f_21 = c/Lambda;  // rest frame transition frequency
  const float k_B = 1.38e-23; // Boltzmann constant
  const float Hubble_fac = 3.24e-20;  // conversion factor from km/s/Mpc to s^-1
  const float msun_to_kg = 1.99e30;
  const double mpc3_to_m3 = pow(3.086e22, 3.0);

  // double fac = 3*h_P*pow(c,3)*A_10*pow(1+z,2);
  // fac /= 32*M_PI*m_H*k_B*pow(f_21,2)*Hubble(z)*Hubble_fac;

  double fac = 3*h_P*pow(c,3)*A_21*pow(1+z,2)*msun_to_kg/mpc3_to_m3;
  fac /= 32*M_PI*m_H*k_B*pow(f_21,2)*Hubble(z)*Hubble_fac;

  // std::cout<<fac<<std::endl;

  uint_fast64_t ii, jj, kk;

  #pragma omp parallel for num_threads(4) collapse(2) private (ii, jj, kk)
    for (ii = 0; ii < N_cell_x; ++ii)
      for (jj = 0; jj < N_cell_x; ++jj)
        for (kk = 0; kk < N_cell_x; ++kk)
          Mapp[Ind(ii, jj, kk)] *= fac * scaling_unit;
}

void HI_Brightness_Temp_Navarro(std::valarray<float> &Mapp)
// returns value in kelvin; Villaescusa-Navarro et al. 2018
// matches with Spinelli et al. 2020 power spectrum
{
  const float G = 6.67e-11; // gravitational constant
  const float Hubble_fac = 3.24e-20;  // conversion factor from km/s/Mpc to s^-1
  const float msun_to_kg = 1.99e30;
  const double mpc3_to_m3 = pow(3.086e22, 3.0);
  float rho_c = 3 * pow(Hubble(z)*Hubble_fac,2) / (8*M_PI*G); // critical density at redshift z (SI units)
  double fac = 18.9 * pow(h,2) * pow(1.0+z,2) * msun_to_kg / mpc3_to_m3;
  fac /= Hubble(z) * rho_c;

  int_fast64_t ii, jj, kk;

  #pragma omp parallel for num_threads(4) collapse(2) private (ii, jj, kk)
    for (ii = 0; ii < N_cell_x; ++ii)
      for (jj = 0; jj < N_cell_x; ++jj)
        for (kk = 0; kk < N_cell_x; ++kk)
          Mapp[Ind(ii, jj, kk)] *= fac * scaling_unit;
}
