#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define IRQ_KEYBOARD    1

#define KBC_STATUS_REG  0x64
#define KBC_OUT_CMD     0x60
#define KBC_OUT_BUF     0x60

#define KBC_OBF         BIT(0)
#define KBC_AUX         BIT(5)
#define KBC_TIMEOUT_ERR BIT(6)
#define KBC_PARITY_ERR  BIT(7)

#define ESC_BREAKCODE   0x81
#define TWO_BYTE_CODE   0xE0

#define DELAY_US 20000

#endif /* _LCOM_I8042_H_ */
