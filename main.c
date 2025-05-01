#include <SFML/Graphics.h>
#include <stdbool.h>
#include "interfaces/pauseMenu.h"

int main(void)
{
    sfVideoMode mode = {800, 600, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "Tela Preta - CSFML", sfClose, NULL);
    sfFont* font = sfFont_createFromFile("./assets/fontes/Arial.ttf");
    if (!window) return 1;

    while (sfRenderWindow_isOpen(window))
    {

        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }
        
        sfRenderWindow_clear(window, sfBlack);
        drawMenu(window, font);

        sfRenderWindow_display(window);
    }

    sfRenderWindow_destroy(window);

    return 0;
}