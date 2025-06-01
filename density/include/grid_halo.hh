#ifndef _GRID_HALO_H
#define _GRID_HALO_H

#include <valarray>
#include <cstdint>

void Coarse_Grid_Buffered(std::valarray<float>&,
							std::valarray<float>&,
							std::valarray<float>&,
							std::valarray<float>&,
							uint32_t,
							uint32_t
						);

#endif
