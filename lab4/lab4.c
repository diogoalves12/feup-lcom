// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab4.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    uint8_t bit_no;
    if (mouse_subscribe_int(&bit_no) != 0) return 1;

    if (mouse_enable_data_reporting_custom() != 0) {
      mouse_unsubscribe_int();
      return 1;
    }

    uint32_t irq_set = BIT(bit_no);
    int ipc_status;
    int r;
    message msg;
    uint8_t packet_bytes[3];
    uint8_t index = 0;
    uint32_t packets_read = 0;

    while (packets_read < cnt) {
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d\n", r);
        continue;
      }

      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
            if (msg.m_notify.interrupts & irq_set) {
              mouse_ih();

              if (mouse_get_error()) break;

              if (mouse_sync_byte(mouse_get_byte(), packet_bytes, &index)) {
                struct packet pp;
                mouse_parse_packet_bytes(packet_bytes, &pp);
                mouse_print_packet(&pp);
                packets_read++;
              }
            }
            break;
          default:
            break;
        }
      }
    }

    if (mouse_disable_data_reporting() != 0) {
      mouse_unsubscribe_int();
      return 1;
    }

    if (mouse_unsubscribe_int() != 0) return 1;

    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}
