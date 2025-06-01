#include <cstdint>
#include <fstream>
#include <iostream>

double Lambda;  // wavelength of the line in metres
float cnv;  // conversion of halo mass to solar mass units
float scaling_unit; // scaling unit for the output intensity maps
float h;  // Hubble constant in units of 100 km/s/Mpc
float Omega_m;  // matter density parameter
float L_cMpc; // box size in comoving Mpc

uint64_t N_cell_x_orig; // original number of cells in the grid
uint64_t N_cell_x;  // number of cells in the grid after coarse-gridding
uint32_t Buf_sz;  // buffer size for reading halos
uint32_t rsd_flag;  // redshift space distortion flag; 0 for no RSD, 1 for RSD

void Read_Param(const char *file) {

  if (file == nullptr) {

    std::cerr << "Error in Read_Param()!" << ' '
              << "Parameter file not given!\n";

    exit(EXIT_FAILURE);
  }

  std::ifstream inFile{file};

  if (inFile.is_open() == false) {

    std::cerr << "Error in Read_Param()!" << ' '
              << "Cannot open parameter file!\n";

    exit(EXIT_FAILURE);
  }

  inFile >> h >> Omega_m >> Lambda >> L_cMpc >> N_cell_x_orig >> N_cell_x >>
      Buf_sz >> cnv >> scaling_unit >> rsd_flag;

  inFile.close();

} // End of Read_Param()
