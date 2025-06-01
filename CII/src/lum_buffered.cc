#include <cmath>
#include <cstdint>
#include <omp.h>
#include <valarray>
#include <iostream>
#include <limits>

extern float h;
extern float Omega_m;
extern float cnv;
extern float z;

inline double Hubble (float zz)
{
  return 100 * 3.24e-20 * h *
         sqrt(Omega_m * powf(1.0 + zz, 3.0) + (1.0 - Omega_m));
}

void Cii_Lum_Buffered_R22 (std::valarray<float> &Lum, uint32_t buff_sz)
{
  uint_fast64_t i;

  // const float a = 0.8475, b = 7.2203; // Silva et al. 2015, model-m1 (De Looze et al. 2014)
  const float a = 1.14, b = 6.76; // Romano et al. 2022 (A&A 660, A14))

  #pragma omp parallel for num_threads(4)
    for (i = 0; i < buff_sz; ++i)
      Lum[i] = powf (10.0, a * log10 (Lum[i]) + b);
}

void Cii_Lum_Buffered_L18 (std::valarray<float> &Lum, std::valarray<float> &Zgas, uint32_t buff_sz) // eq.(11) of Lagache et al. 2018
{ // only pass halos with nonzero metallicity
  uint_fast64_t i; 
  float infinity = std::numeric_limits<float>::infinity();
  float Zterm, crossterm;

  #pragma omp parallel for num_threads(4)
    for (i = 0; i < buff_sz; ++i) {
      Zgas[i] = 1.12*log10(Zgas[i]) + 9.06;  // fit to the data in Table 1. of Lagache et al. 2018
      crossterm = 0.063*log10(Lum[i])*log10(Zgas[i]);
      crossterm *= (crossterm == infinity) ? -1 : 1;

      Lum[i] = 7.5 + 0.67*log10(Lum[i]) - 0.13*log10(Zgas[i]) - 0.79*powf(log10(Zgas[i]),2) + crossterm;
      Lum[i] = powf(10.0, Lum[i]);
    }
}

// void CO_Lum_Buffered (std::valarray<float> &Lum, uint32_t buff_sz)
// {
//   uint_fast64_t i;

//   const float del_mf = 1.0;
//   const float del_mf_fac = 1.0 / (del_mf * 1e-10);

//   const float alpha = 1.11;
//   const float alpha_inv = 1.0 / alpha;
//   const float beta = 0.6;

//   const int8_t J = 2;
//   const float fac = 4.95e-5 * powf (1.0 * J, 3.0);

//   #pragma omp parallel for num_threads(4)
//     for (i = 0; i < buff_sz; ++i)
//       {
//         Lum[i] *= del_mf_fac; // SFR_to_L-IR
//         Lum[i] = powf (10.0, (log10 (Lum[i]) - beta) * alpha_inv);
//         // L-IR_to_L-CO_prime

//         Lum[i] *= fac; // CO(j->j-1) line luminosity
//       }
// } // End of CO_Lum_Buffered()

void CO_Lum_Buffered_Li16 (std::valarray<float> &Lum, uint32_t buff_sz)
{
  uint_fast64_t i;

  const float del_mf = 1.0;
  const float del_mf_fac = 1.0 / (del_mf * 1e-10);  // Behroozi et al. 2013, ApJ, 770, 57

  const float alpha = 1.37;
  const float beta = -1.74; // Li et al. 2016, Carilli & Walter 2013 (fiducial)

  //const float alpha = 0.81;
  //const float beta = 0.54; // Sargent et al. 2014

  const float alpha_inv = 1.0 / alpha;

  // const float line_ratio = 0.76; // L_CO(j->j-1) / L_CO(1-0) Fonseca et al. 2016
  const float line_ratio = 1.0;

  const int8_t J = 1;
  const float fac = 4.95e-5 * powf (1.0 * J, 3.0);

#pragma omp parallel for num_threads(4)
  for (i = 0; i < buff_sz; ++i)
    {
      Lum[i] *= del_mf_fac; // SFR_to_L-IR
      Lum[i] = powf (10, (log10 (Lum[i]) - beta) * alpha_inv); // L-IR_to_L-CO_prime (1-0)
      Lum[i] *= line_ratio;
      Lum[i] *= fac;
    }
}

void CO_Lum_Buffered_Li16K20 (std::valarray<float> &Lum, uint32_t buff_sz)
{
  uint_fast64_t i;

  // model from Li et al. 2016, Keating et al. 2020

  const float del_mf = 1.0;
  const float del_mf_fac = 1.0 / (del_mf * 1e-10);  // Behroozi et al. 2013, ApJ, 770, 57

  const float alpha = 1.27;
  const float beta = -1.0;  // Kamenetzky et al. 2016, ApJ, 829, 93

  const float alpha_inv = 1.0 / alpha;

  // const float line_ratio = 0.76; // L_CO(j->j-1) / L_CO(1-0) Fonseca et al. 2016
  const float line_ratio = 1.0;

  const int8_t J = 1;
  const float fac = 4.95e-5 * powf (1.0 * J, 3.0);

#pragma omp parallel for num_threads(4)
  for (i = 0; i < buff_sz; ++i)
    {
      Lum[i] *= del_mf_fac; // SFR_to_L-IR
      Lum[i] = powf (10, (log10 (Lum[i]) - beta) * alpha_inv); // L-IR_to_L-CO_prime (1-0)
      Lum[i] *= line_ratio;
      Lum[i] *= fac;
    }
}

void CO_Lum_Buffered_Y22 (std::valarray<float> &Lum, uint32_t buff_sz)
{
  uint_fast64_t i;

  float N = -6.855 + 0.2366*z - 0.05013*z*z;
  N = powf(10, N);
  float M1 = 12.13 - 0.1678*z;
  M1 = powf(10, M1);
  const float alpha = 1.642 + 0.1663*z - 0.03238*z*z;
  const float beta = 1.77*exp(-z/2.72) - 0.0827;  // Table 1 of Yang et al. 2022, ApJ, 929, 140

  #pragma omp parallel for num_threads(4)
    for (i = 0; i < buff_sz; ++i)
      {
        Lum[i] *= cnv;
        Lum[i] = 2*N*Lum[i] / (powf(Lum[i]/M1,-alpha) + powf(Lum[i]/M1,beta));
      }
}

void HI_mass_Buffered (std::valarray<float> &Lum, uint32_t buff_sz) // HI mass in halo (M_sun)
{
  uint_fast64_t i;

  const float HI_frac = 0.7;
  // std::cout<<"cnv="<<cnv<<std::endl;
  #pragma omp parallel for num_threads(4)
    for (i = 0; i < buff_sz; ++i)
      Lum[i] *= HI_frac * cnv;
      // std::cout<<Lum[i]<<"\t";
}
