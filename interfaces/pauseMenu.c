#include "pauseMenu.h"
#include <SFML/Graphics.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void menu_init(PauseMenu *menu){
    menu->window = sfRenderWindow_create((sfVideoMode){800, 600, 32}, "Pause window", sfClose, NULL);

    menu->font = sfFont_createFromFile("./assets/fontes/Arial.ttf");
    menu->selected = 0;
    menu->btn_idle = sfTexture_createFromFile("./assets/sprites/idle.png", NULL);
    menu->btn_clicked = sfTexture_createFromFile("./assets/sprites/Click.png", NULL);
    menu->bg_texture = sfTexture_createFromFile("./assets/sprites/bg_teste.jpg", NULL);

    menu->bg = sfSprite_create();
    
    sfSprite_setTexture(menu->bg, menu->bg_texture, sfTrue);
    

    const char* texts[NUM_OPCOES] = {"RESUME", "SCOREBOARD", "OPTIONS", "QUIT"};
    const int x_pos[NUM_OPCOES] = {335, 300, 330, 355};

    for(int i = 0; i < NUM_OPCOES; i++){
        menu->buttons[i] = sfSprite_create();

        sfSprite_setTexture(menu->buttons[i], menu->btn_idle, sfTrue);
        sfSprite_setScale(menu->buttons[i], (sfVector2f){2.0, 1.5});
        sfSprite_setPosition(menu->buttons[i], (sfVector2f){270, 80 + i * 110});

        menu->options[i] = sfText_create();

        sfText_setString(menu->options[i], texts[i]);
        sfText_setFont(menu->options[i], menu->font);
        sfText_setCharacterSize(menu->options[i], 25);
        sfText_setFillColor(menu->options[i], sfWhite);
        sfText_setPosition(menu->options[i], (sfVector2f){x_pos[i], 100 + i * 110 + 5});
    }
    sfText_setFillColor(menu->options[0], sfYellow);
}

void menu_run(PauseMenu* menu) {
    while (sfRenderWindow_isOpen(menu->window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(menu->window, &event)) {
            if (event.type == sfEvtClosed){
                sfRenderWindow_close(menu->window);
            }    

            if (event.type == sfEvtKeyPressed) {
                sfSprite_setTexture(menu->buttons[menu->selected], menu->btn_idle, sfTrue);
                if (event.key.code == sfKeyDown && menu->selected < NUM_OPCOES - 1) {
                    sfText_setFillColor(menu->options[menu->selected], sfWhite);
                    menu->selected++;
                    sfText_setFillColor(menu->options[menu->selected], sfYellow);
                }
                if (event.key.code == sfKeyUp && menu->selected > 0) {
                    sfText_setFillColor(menu->options[menu->selected], sfWhite);
                    menu->selected--;
                    sfText_setFillColor(menu->options[menu->selected], sfYellow);
                }
                if (event.key.code == sfKeyEnter) {
                    printf("Selecionado: %d\n", menu->selected);
                    sfSprite_setTexture(menu->buttons[menu->selected], menu->btn_clicked, sfTrue);

                    if (menu->selected == 3)
                        sfRenderWindow_close(menu->window);
                }
            }
        }

        sfRenderWindow_clear(menu->window, sfBlack);
        sfRenderWindow_drawSprite(menu->window, menu->bg, NULL);
        for (int i = 0; i < NUM_OPCOES; i++) {
            sfRenderWindow_drawSprite(menu->window, menu->buttons[i], NULL);
            sfRenderWindow_drawText(menu->window, menu->options[i], NULL);
        }
        sfRenderWindow_display(menu->window);
    }
}

void menu_destroy(PauseMenu* menu) {
    for (int i = 0; i < NUM_OPCOES; i++) {
        sfText_destroy(menu->options[i]);
    }
    sfFont_destroy(menu->font);
    sfRenderWindow_destroy(menu->window);
}