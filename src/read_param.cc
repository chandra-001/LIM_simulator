#include <cstdint>
#include <fstream>
#include <iostream>

double Lambda;
float cnv;  // conversion of halo mass to solar mass units
float scaling_unit;
float h;
float Omega_m;
float L_cMpc;

uint64_t N_cell_x_orig;
uint64_t N_cell_x;
uint32_t Buf_sz;

void Read_Param(const char *file) {

  if (file == nullptr) {

    std::cerr << "Error in Read_Param()!" << ' '
              << "Paramter file not given!\n";

    exit(EXIT_FAILURE);
  }

  std::ifstream inFile{file};

  if (inFile.is_open() == false) {

    std::cerr << "Error in Read_Param()!" << ' '
              << "Cannot open parameter file!\n";

    exit(EXIT_FAILURE);
  }

  inFile >> h >> Omega_m >> Lambda >> L_cMpc >> N_cell_x_orig >> N_cell_x >>
      Buf_sz >> cnv >> scaling_unit;

  inFile.close();

} // End of Read_Param()
