#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"

static int mouse_hook_id = MOUSE_IRQ;
static uint8_t mouse_byte = 0;
static bool mouse_error = false;

static int (mouse_read_ack)(uint8_t *ack) {
  uint8_t status;

  if (ack == NULL) return -1;

  for (int i = 0; i < MAX_TRIES; i++) {
    if (util_sys_inb(KBC_STAT_REG, &status) != 0) return -1;

    if (status & KBC_OBF) {
      if (util_sys_inb(KBC_OUT_BUF, ack) != 0) return -1;
      if ((status & (KBC_PARITY | KBC_TIMEOUT)) != 0) return -1;
      if ((status & KBC_AUX) == 0) return -1;
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));
  }

  return -1;
}

int (mouse_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return -1;

  *bit_no = mouse_hook_id;

  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != 0) return -1;

  return 0;
}

int (mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id) != 0) return -1;

  return 0;
}

void (mouse_ih)() {
  uint8_t status;
  uint8_t data;
  mouse_error = true;

  if (util_sys_inb(KBC_STAT_REG, &status) != 0) return;

  if (status & KBC_OBF) {
    if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

    if ((status & (KBC_PARITY | KBC_TIMEOUT)) == 0 && (status & KBC_AUX) != 0) {
      mouse_byte = data;
      mouse_error = false;
    }
  }
}

uint8_t (mouse_get_byte)() {
  return mouse_byte;
}

bool (mouse_get_error)() {
  return mouse_error;
}

int (mouse_write_command)(uint8_t command) {
  uint8_t status;
  uint8_t ack;

  for (int retry = 0; retry < MAX_COMMAND_RETRIES; retry++) {
    for (int i = 0; i < MAX_TRIES; i++) {
      if (util_sys_inb(KBC_STAT_REG, &status) != 0) return -1;

      if ((status & KBC_IBF) == 0) {
        if (sys_outb(KBC_CMD_REG, WRITE_TO_MOUSE) != 0) return -1;
        break;
      }

      tickdelay(micros_to_ticks(DELAY_US));
      if (i == MAX_TRIES - 1) return -1;
    }

    for (int i = 0; i < MAX_TRIES; i++) {
      if (util_sys_inb(KBC_STAT_REG, &status) != 0) return -1;

      if ((status & KBC_IBF) == 0) {
        if (sys_outb(KBC_IN_BUF, command) != 0) return -1;
        break;
      }

      tickdelay(micros_to_ticks(DELAY_US));
      if (i == MAX_TRIES - 1) return -1;
    }

    if (mouse_read_ack(&ack) != 0) return -1;

    if (ack == MOUSE_ACK) return 0;
    if (ack != MOUSE_NACK && ack != MOUSE_ERROR) return -1;
  }

  return -1;
}

int (mouse_enable_data_reporting_custom)() {
  return mouse_write_command(ENABLE_DATA_REPORTING);
}

int (mouse_disable_data_reporting)() {
  return mouse_write_command(DISABLE_DATA_REPORTING);
}

bool (mouse_sync_byte)(uint8_t byte, uint8_t packet[3], uint8_t *index) {
  if (packet == NULL || index == NULL) return false;

  if (*index == 0 && (byte & MOUSE_FIRST_BYTE) == 0) return false;

  packet[*index] = byte;
  (*index)++;

  if (*index == 3) {
    *index = 0;
    return true;
  }

  return false;
}

void (mouse_parse_packet_bytes)(const uint8_t bytes[3], struct packet *pp) {
  if (bytes == NULL || pp == NULL) return;

  pp->bytes[0] = bytes[0];
  pp->bytes[1] = bytes[1];
  pp->bytes[2] = bytes[2];

  pp->lb = bytes[0] & BIT(0);
  pp->rb = bytes[0] & BIT(1);
  pp->mb = bytes[0] & BIT(2);

  pp->x_ov = bytes[0] & BIT(6);
  pp->y_ov = bytes[0] & BIT(7);

  pp->delta_x = (bytes[0] & BIT(4)) ? (int16_t) (0xFF00 | bytes[1]) : (int16_t) bytes[1];
  pp->delta_y = (bytes[0] & BIT(5)) ? (int16_t) (0xFF00 | bytes[2]) : (int16_t) bytes[2];
}
