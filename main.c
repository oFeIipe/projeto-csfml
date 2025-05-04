#include <SFML/Graphics.h>
#include "interfaces/pauseMenu.h"

int main() {
    PauseMenu menu;
    menu_init(&menu);
    menu_run(&menu);
    menu_destroy(&menu);
    return 0;
}
