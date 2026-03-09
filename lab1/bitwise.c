#include "bitwise.h"
#include <stdarg.h>

uint8_t clear(uint8_t msk, int pos) {
  uint8_t mask = (uint8_t)(1u << pos);
  return msk & (uint8_t)(0xFF ^ mask);
}

uint8_t set(uint8_t msk, int pos) { 
  uint8_t mask = (uint8_t)(1u << pos);
  return msk | mask;
}

bool is_set(uint8_t msk, int pos) { 
  uint8_t mask = (uint8_t)(1u << pos);
  return (msk & mask) != 0;
}

uint8_t lsb(uint16_t wide_msk) {
  return (uint8_t)(wide_msk & 0xFF);
}

uint8_t msb(uint16_t wide_msk) {
  return (uint8_t)((wide_msk >> 8) & 0xFF);
}

uint8_t mask(int pos, ...) {
  uint8_t result = 0;
  va_list args;

  va_start(args, pos);
  while (pos != MSK_END) {
    if (pos >= 0 && pos < 8) {
      uint8_t bit_msk = (uint8_t)(1u << pos);
      result |= bit_msk;
    }
    pos = va_arg(args, int);
  }
  va_end(args);

  return result;
}
