#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  if (timer > 2) return -1;
  if(st == NULL) return -1;

  uint8_t rb_cmd  = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if(sys_outb(TIMER_CTRL, rb_cmd) != 0) return -1;

  int port;
  if(timer == 0) port = TIMER_0;
  else if(timer == 1) port = TIMER_1;
  else port = TIMER_2;

  if(util_sys_inb(port, st) != 0) return -1;

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  if (timer > 2) return -1;

  union timer_status_field_val val;

  switch (field)
  {
  case tsf_all:
    val.byte = st;
    break;

  case tsf_initial: {
    uint8_t init  = (st >> 4) & 0x03;
    switch (init)
    {
    case 0:
      val.in_mode = INVAL_val;
      break;
    case 1:
      val.in_mode = LSB_only;
      break;
    case 2:
      val.in_mode = MSB_only;
      break;
    case 3:
      val.in_mode = MSB_after_LSB;
      break;
    default:
      return -1;
    }
    break;
  }

  case tsf_mode:
    val.count_mode = (st >> 1) & 0x07;
    if(val.count_mode == 6) val.count_mode = 2;
    else if (val.count_mode == 7) val.count_mode = 3;
    break;

  case tsf_base:
    val.bcd = (st & 0x01);
    break;

  default:
    return -1;
  }

  if (timer_print_config(timer, field, val) != 0) return -1;

  return 0;
}
