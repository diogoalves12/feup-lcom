#ifndef _LCOM_LAB4_MOUSE_H_
#define _LCOM_LAB4_MOUSE_H_

#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
uint8_t (mouse_get_byte)();
bool (mouse_get_error)();
int (mouse_read_pending_byte)(uint8_t *byte);

int (mouse_write_command)(uint8_t command);
int (mouse_enable_data_reporting_custom)();
int (mouse_disable_data_reporting)();

bool (mouse_sync_byte)(uint8_t byte, uint8_t packet[3], uint8_t *index);
void (mouse_parse_packet_bytes)(const uint8_t bytes[3], struct packet *pp);

#endif /* _LCOM_LAB4_MOUSE_H_ */
