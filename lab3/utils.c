#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = val & 0xFF; // Get the last 8 bits
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = (val >> 8) & 0xFF; // Shift right 8 bits and get them
  return 0;
}

int sys_inb_counter = 0;

int(util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) return 1;
  uint32_t val32;
  
  if (sys_inb(port, &val32) != 0) return 1;
  *value = (uint8_t) val32;

#ifdef LAB3
  sys_inb_counter++;
#endif

  return 0;
}
