#include <lcom/lcf.h>
#include "i8042.h"

int hook_id_kbd = 1;      
uint8_t scancode = 0;     
bool error_found = false; 

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id_kbd;
  return sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd);
}

int (keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id_kbd);
}

void (kbc_ih)() {
  uint8_t status;
  error_found = false;

  if (util_sys_inb(KBC_STATUS_REG, &status) != 0) {
    error_found = true;
    return;
  }

  if (status & KBC_OBF) {
    if (util_sys_inb(KBC_OUT_BUF, &scancode) != 0) {
      error_found = true;
      return;
    }
    if ((status & KBC_PARITY_ERR) || (status & KBC_TIMEOUT_ERR)) {
      error_found = true; 
    }
  } else {
    error_found = true;
  }
}

int (keyboard_poll)(uint8_t *codigo) {
    uint8_t status;
    while (1) {
        if (util_sys_inb(KBC_STATUS_REG, &status) != 0) return 1;
        if (status & KBC_OBF) {
            if (util_sys_inb(KBC_OUT_BUF, codigo) != 0) return 1;

            if ((status & KBC_PARITY_ERR) || (status & KBC_TIMEOUT_ERR)) {
                return 1;
            }
            return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
}
