#ifndef _CIC_BUFFERED_HH
#define _CIC_BUFFERED_HH

#include <valarray>
#include <cstdint>

void Cloud_in_Cell_Buffered(std::valarray<float>&,
							std::valarray<float>&,
							std::valarray<float>&,
							std::valarray<float>&,
							uint32_t,
							std::valarray<float>& );

void Nearest_grid_point_Buffered(	std::valarray<float>&,
									std::valarray<float>&,
									std::valarray<float>&,
									std::valarray<float>&,
									uint32_t,
									std::valarray<float>& );

#endif
