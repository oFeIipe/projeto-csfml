#include <SFML/Graphics.h>
#include "pauseMenu.h"

#define NUM_OPCOES 4

char menu_options[NUM_OPCOES][20] = {{"RESUME"}, {"SCOREBOARD"}, {"OPTIONS"}, {"QUIT"}};

void drawCenteredText(sfRenderWindow *window, sfFont* font, const char* str, float y_pos) {
    sfText* text = sfText_create();
    sfText_setString(text, str);
    sfText_setFont(text, font);
    sfText_setCharacterSize(text, 25);
    sfText_setFillColor(text, sfWhite);

    sfFloatRect bounds = sfText_getLocalBounds(text);
    float x_pos = (sfRenderWindow_getSize(window).x - bounds.width) / 2.0f;

    sfText_setPosition(text, (sfVector2f){x_pos, y_pos});
    sfRenderWindow_drawText(window, text, NULL);
    sfText_destroy(text);
}

void drawCenteredButtons(sfRenderWindow *window, float y_pos){
    sfSprite *sprite = sfSprite_create();
    sfTexture *button = sfTexture_createFromFile("./assets/sprites/click.png", NULL);
    sfSprite_setTexture(sprite, button, sfTrue);
    sfSprite_setScale(sprite, (sfVector2f){2.0, 1.5});

    sfFloatRect bounds = sfSprite_getLocalBounds(sprite);
    float x_pos = (sfRenderWindow_getSize(window).x - bounds.width) / 2.0f;

    sfSprite_setPosition(sprite, (sfVector2f){x_pos - 60, y_pos - 25});

    sfRenderWindow_drawSprite(window, sprite, NULL);
}

void drawMenu(sfRenderWindow *window, sfFont *font){
    int y_pos;

    for(int i = 0, y_pos = 100; i < NUM_OPCOES; i++, y_pos += 120){
        drawCenteredButtons(window, y_pos);
        drawCenteredText(window, font, menu_options[i], y_pos);
    }
}

/*int main() {
    sfVideoMode mode = {800, 600, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "Texto Centralizado", sfClose, NULL);
    sfFont* font = sfFont_createFromFile("./assets/fontes/Arial.ttf");
    

    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) sfRenderWindow_close(window);
    }

    sfRenderWindow_clear(window, sfBlack);

    drawMenu(window, font);

    sfRenderWindow_display(window);
    }

    sfRenderWindow_destroy(window);
    return 0;
}*/