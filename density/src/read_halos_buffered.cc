#include <array>
#include <cstdint>
#include <fstream>
#include <omp.h>
#include <string>
#define num_files 6

extern std::string Dir;

char Buffer[num_files][4096];

void File_Handler (std::array<std::ifstream, num_files> &in, int8_t flag) {

  std::string File[] = {"Pos_x.bin", "Pos_y.bin", "Pos_z.bin", "Vel_z.bin", "Mass.bin", "Metallicity.bin"};  // for density maps
  // std::string File[] = {"Pos_x.bin", "Pos_y.bin", "Pos_z.bin", "Vel_z.bin", "SFR.bin", "Metallicity.bin"};  // for CII and CO maps
  // std::string File[] = {"Pos_x.bin", "Pos_y.bin", "Pos_z.bin", "Vel_z.bin", "H_mass.bin", "Metallicity.bin"};  // for 21-cm maps; Metallicity.bin is not being used here

  if (flag == 0) {

    for (uint8_t i = 0; i < num_files; ++i) {
      in[i].rdbuf()-> pubsetbuf(Buffer[i], 4096);
      in[i].open(Dir + '/' + File[i], std::ios_base::binary);
    }
  } 
  
  else {

    for (uint8_t i = 0; i < num_files; ++i)
      in[i].close();
  }
}

void Read_Halos_Buffered (std::array<std::ifstream, num_files> &in,
                          std::array<char *, num_files> &buffer, uint32_t buff_sz)  {
  uint8_t i = 0;
  uint_fast32_t p;

  #pragma omp parallel for num_threads(4)
    for (i = 0; i < num_files; ++i)
      in[i].read(buffer[i], sizeof(float) * buff_sz);
}
