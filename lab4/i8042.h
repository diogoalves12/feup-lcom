#ifndef _LCOM_LAB4_I8042_H_
#define _LCOM_LAB4_I8042_H_

#include <lcom/lcf.h>

/* KBC ports */
#define KBC_STAT_REG 0x64
#define KBC_CMD_REG  0x64
#define KBC_IN_BUF   0x60
#define KBC_OUT_BUF  0x60

/* KBC status register */
#define KBC_OBF     BIT(0)
#define KBC_IBF     BIT(1)
#define KBC_AUX     BIT(5)
#define KBC_TIMEOUT BIT(6)
#define KBC_PARITY  BIT(7)

/* Mouse IRQ */
#define MOUSE_IRQ 12

/* KBC commands */
#define WRITE_TO_MOUSE 0xD4

/* Mouse commands */
#define ENABLE_DATA_REPORTING  0xF4
#define DISABLE_DATA_REPORTING 0xF5
#define SET_SAMPLE_RATE        0xF3

/* Mouse replies */
#define MOUSE_ACK   0xFA
#define MOUSE_NACK  0xFE
#define MOUSE_ERROR 0xFC

/* Packet helpers */
#define MOUSE_FIRST_BYTE BIT(3)

/* Polling/retry timing */
#define DELAY_US 20000
#define MAX_TRIES 10
#define MAX_COMMAND_RETRIES 3

#endif /* _LCOM_LAB4_I8042_H_ */
