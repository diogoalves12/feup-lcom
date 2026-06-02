#ifndef PROJECT_GAME_H
#define PROJECT_GAME_H

#include <stdint.h>
#include <stdbool.h>

#include "arena.h"
#include "combat.h"
#include "game_input.h"
#include "game_over.h"
#include "game_state.h"
#include "keyboard_input.h"
#include "menu.h"
#include "mouse_input.h"
#include "pause_menu.h"
#include "player.h"

// Central game context. Owns the current state, devices, entities and menus.
typedef struct {
  GameState        state;
  uint32_t         frame_counter;
  KeyboardInput    keyboard;
  MouseInput       mouse;
  uint8_t          mouse_packet[3];
  uint8_t          mouse_packet_idx;
  GameInputActions actions;
  Arena            arena;
  Player           player1;
  Player           player2;
  CombatState      combat;
  // prev_p1_shoot / prev_p2_shoot detect a new shot press (rising edge).
  bool             prev_p1_shoot;
  bool             prev_p2_shoot;
  MenuState        menu;
  PauseMenuState   pause_menu;
  GameOverState    game_over;
  bool             restart_requested;
} Game;

int game_init(Game *game);
int game_run(Game *game);
int game_shutdown(Game *game);

#endif // PROJECT_GAME_H
