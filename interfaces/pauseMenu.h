#define PAUSE_MENU_H
#include <SFML/Graphics.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include<windows.h>

#define NUM_OPCOES 4

typedef struct {
    sfRenderWindow* window;
    sfFont* font;
    sfText *options[NUM_OPCOES];
    sfSprite *buttons[NUM_OPCOES];
    sfSprite *bg;
    sfTexture *btn_idle;
    sfTexture *btn_clicked;
    sfTexture *bg_texture;
    int selected;
} PauseMenu;

void menu_init(PauseMenu* menu);
void menu_run(PauseMenu* menu);
void menu_destroy(PauseMenu* menu);
