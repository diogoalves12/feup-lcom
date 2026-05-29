#ifndef PROJECT_UI_MENU_H
#define PROJECT_UI_MENU_H

typedef enum {
  MENU_RESULT_START_GAME,
  MENU_RESULT_EXIT_GAME
} MenuResult;

MenuResult menu_loop(void);

#endif
