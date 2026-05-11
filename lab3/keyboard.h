#ifndef LCOM_LAB3_KEYBOARD_H
#define LCOM_LAB3_KEYBOARD_H

#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)();
void (kbc_ih)();
int (keyboard_poll)(uint8_t *codigo);
uint8_t (keyboard_get_scancode)();
bool (keyboard_has_error)();

#endif
