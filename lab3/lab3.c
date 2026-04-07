#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include <lcom/timer.h>
#include "i8042.h"

extern uint8_t scancode;
extern bool error_found;
extern int sys_inb_counter;
int keyboard_subscribe_int(uint8_t *bit_no);
int keyboard_unsubscribe_int();
void (kbc_ih)();
int (keyboard_poll)(uint8_t *codigo);
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  if (keyboard_subscribe_int(&bit_no) != 0) return 1;

  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);

  uint8_t bytes[2];
  uint8_t size = 0;

  while (scancode != ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            
            kbc_ih(); 
            if (error_found) continue;

            bytes[size] = scancode;
            size++;

            if (scancode == TWO_BYTE_CODE) {
                continue; 
            }

            bool make = !(scancode & BIT(7));
            kbd_print_scancode(make, size, bytes);
            
            size = 0; 
          }
          break;
        default:
          break;
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0) return 1;
  kbd_print_no_sysinb(sys_inb_counter);

  return 0;
}

int(kbd_test_poll)() {
  uint8_t bytes[2];
  uint8_t size = 0;
  uint8_t poll_scancode = 0;

  while (poll_scancode != ESC_BREAKCODE) {
    
    if (keyboard_poll(&poll_scancode) != 0) {
        continue;
    }

    bytes[size] = poll_scancode;
    size++;

    if (poll_scancode == TWO_BYTE_CODE) {
        continue;
    }

    bool make = !(poll_scancode & BIT(7));
    kbd_print_scancode(make, size, bytes);
            
    size = 0; 
  }

  kbd_print_no_sysinb(sys_inb_counter);

  uint8_t command_byte;
  sys_outb(KBC_STATUS_REG, 0x20);
  util_sys_inb(KBC_OUT_BUF, &command_byte);
  command_byte = command_byte | BIT(0);
  sys_outb(KBC_STATUS_REG, 0x60);
  sys_outb(KBC_OUT_BUF, command_byte);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t kbd_bit_no;
  uint8_t timer_bit_no;
  
  if (keyboard_subscribe_int(&kbd_bit_no) != 0) return 1;
  if (timer_subscribe_int(&timer_bit_no) != 0) return 1;

  int ipc_status;
  message msg;
  uint32_t kbd_irq_set = BIT(kbd_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  uint8_t bytes[2];
  uint8_t size = 0;
  
  int seconds = 0;
  timer_counter = 0;

  while (scancode != ESC_BREAKCODE && seconds < n) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          
          if (msg.m_notify.interrupts & kbd_irq_set) {
            kbc_ih(); 
            if (error_found) continue;

            bytes[size] = scancode;
            size++;

            if (scancode == TWO_BYTE_CODE) continue; 

            bool make = !(scancode & BIT(7));
            kbd_print_scancode(make, size, bytes);
            size = 0; 

            timer_counter = 0;
            seconds = 0;
          }

          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            
            if (timer_counter % 60 == 0) {
              seconds++;
            }
          }
          break;
          
        default:
          break;
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  
  kbd_print_no_sysinb(sys_inb_counter);

  return 0;
}
