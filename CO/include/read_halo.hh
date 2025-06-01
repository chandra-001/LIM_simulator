#ifndef _READ_HALO_H
#define _READ_HALO_H
#define num_files 6

#include <string>
#include <valarray>
#include <array>

void File_Handler ( std::array<std::ifstream,num_files>&,
					int8_t);

void  Read_Halos_Buffered ( std::array<std::ifstream,num_files>&,
							std::array<char*,num_files>&,
							uint32_t);
#endif
