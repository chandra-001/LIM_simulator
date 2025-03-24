#ifndef _LUM_BUFFERED_HH
#define _LUM_BUFFERED_HH

#include <valarray>
#include  <cstdint>

void Cii_Lum_Buffered_R22 (std::valarray<float>&, uint32_t);
void Cii_Lum_Buffered_L18 (std::valarray<float>&, std::valarray<float>&, uint32_t);
void CO_Lum_Buffered_Li16 (std::valarray<float>&, uint32_t);
void CO_Lum_Buffered_Li16K20 (std::valarray<float>&, uint32_t);
void CO_Lum_Buffered_Y22 (std::valarray<float>&, uint32_t);
void HI_mass_Buffered (std::valarray<float>&, uint32_t);

#endif
