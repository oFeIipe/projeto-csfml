#include <SFML/Graphics.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define PI 3.14159265359

typedef struct
{   
    sfTexture* playerTexture;
    sfSprite* playerSprite;
    sfVector2f mousePos;
    sfVector2f playerCenterPos;
    sfVector2f aimDirection;
    sfVector2f aimDirectionNormalize;
}Player;

typedef struct
{
    sfCircleShape* shape;
    sfVector2f currentVelocity;
    float maxSpeed;
}Bullet;

typedef struct {
    int capacity;
    int size;
    Bullet **bullets;
}BulletVector;

typedef struct{
    sfTexture *texture;
    sfSprite *shape;
    sfVector2f position;
}Enemy;

typedef struct {
    int capacity;
    int size;
    Enemy **enemies;
}EnemyVector;


typedef struct {
    sfSprite *bg;
    sfSprite *btnExit;
    sfSprite *btnRestart;
    sfText *text[2];
    sfText *textScore;
    sfText *textTime;
    int selected;
} GameOver;

EnemyVector* createEnemyVector(int capacity) {
    EnemyVector* vec = malloc(sizeof(EnemyVector));
    vec->enemies = (Enemy **) malloc(sizeof(Enemy*) * capacity);
    vec->size = 0;
    vec->capacity = capacity;
    return vec;
}

void spawnEnemy(Enemy *enemy, int windowWidth, int windowHeight){
    int side = rand() % 4;

    if(side == 0) {
        enemy->position.x = rand() % windowWidth;
        enemy->position.y = -100;
    }
    if(side == 1) {
        enemy->position.x = rand() % windowWidth;
        enemy->position.y = windowHeight + 100;
    }
    if(side == 2) {
        enemy->position.x = -100;
        enemy->position.y = rand() % windowHeight;
    }
    if(side == 3) {
        enemy->position.x = windowWidth + 100;
        enemy->position.y = rand() % windowHeight;
    }

    sfSprite_setPosition(enemy->shape, enemy->position);
}

Enemy* createEnemy(Player *player, int windowWidth, int windowHeight) {
    Enemy *enemy = malloc(sizeof(Enemy));
    enemy->texture = sfTexture_createFromFile("./assets/sprites/zombie.png", NULL);
    float distance, minDistance = 100;
    if(!enemy->texture){
        printf("Nao foi possivel encontrar a imagem");
        exit(1);
    }
    enemy->shape = sfSprite_create();
    sfSprite_setTexture(enemy->shape, enemy->texture, sfTrue);

    sfSprite_setScale(enemy->shape, (sfVector2f){0.25f, 0.25f});

    spawnEnemy(enemy, windowWidth, windowHeight);
    return enemy;
}

void pushBackEnemy(EnemyVector* vector, Enemy* enemy) {
    if (vector->size >= vector->capacity) {
        vector->capacity *= 2;
        vector->enemies = realloc(vector->enemies, sizeof(Enemy*) * vector->capacity);
    }
    vector->enemies[vector->size++] = enemy;
}

void eraseEnemyAtIndex(EnemyVector* vector, int index) {
    if (index < 0 || index >= vector->size) return;
    
    sfSprite_destroy(vector->enemies[index]->shape);

    for (int i = index; i < vector->size - 1; i++) {
        vector->enemies[i]->shape = vector->enemies[i + 1]->shape;
    }

    vector->size--;
}


void freeEnemyVector(EnemyVector* vector) {
    for (int i = 0; i < vector->size; i++) {
        sfSprite_destroy(vector->enemies[i]->shape);
    }
    free(vector->enemies);
    free(vector);
}
Player *createPlayer(){
    Player *player = malloc(sizeof(Player));

    player->playerTexture = sfTexture_createFromFile("./assets/sprites/player.png", NULL);

    player->playerSprite = sfSprite_create();
    sfSprite_setTexture(player->playerSprite, player->playerTexture, sfTrue);
    sfSprite_setScale(player->playerSprite, (sfVector2f){0.3f, 0.3f});
    sfSprite_setPosition(player->playerSprite, (sfVector2f){600, 450});

    return player;
}

Bullet * createBullet(float radius){
    Bullet *bala = malloc(sizeof(Bullet));

    bala->shape = sfCircleShape_create();
    bala->maxSpeed = 10.f;

    sfCircleShape_setRadius(bala->shape, radius);
    sfCircleShape_setFillColor(bala->shape, sfRed);

    return bala;
}

BulletVector *createBulletVector(int capacity){
    BulletVector *vector = malloc(sizeof(BulletVector));
    if (!vector) {
        printf("Erro ao alocar memória para o vetor!\n");
        exit(1);
    }

    vector->bullets = calloc(capacity, sizeof(Bullet *));
    if (!vector->bullets) {
        free(vector);
        printf("Erro ao alocar memória para os bullets!\n");
        exit(1);
    }

    vector->size = 0;
    vector->capacity = capacity;
    return vector;
}

void pushBulletBack(BulletVector *vector, Bullet *bullet){
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        Bullet **newBullets = realloc(vector->bullets, vector->capacity * sizeof(Bullet *));
        if (!newBullets) {
            printf("Erro: Falha ao realocar memória!\n");
            exit(1);
        }
        vector->bullets = newBullets;
    }
    vector->bullets[vector->size++] = bullet;
}

void eraseBulletAtIndex(BulletVector* vector, int index) {
    if (index < 0 || index >= vector->size) return;

    sfCircleShape_destroy(vector->bullets[index]->shape);

    for (int i = index; i < vector->size - 1; i++) {
        vector->bullets[i] = vector->bullets[i + 1];
    }

    vector->size--;
}


void freeBulletVector(BulletVector *vector) {
    if (vector) {
        for (int i = 0; i < vector->size; i++) {
            if (vector->bullets[i]) {
                sfCircleShape_destroy(vector->bullets[i]->shape);
                free(vector->bullets[i]);
            }
        }
        free(vector->bullets);
        free(vector);
    }
}


void writeHighScore(char *score){
    FILE *file = fopen("Highscore.txt", "w");
    if(file != NULL){
        fprintf(file, "%s",score);
        fclose(file);
    }
}

bool saveHighScore(char *score){
    char textBuffer[10];
    int highScore = 0, currentScore = atoi(score);
    FILE *file = fopen("Highscore.txt", "r");

    if(file == NULL){
        writeHighScore(score);
        return false;
    }

    if(fgets(textBuffer, sizeof(textBuffer), file) != NULL){
        highScore = atoi(textBuffer);
    }
    else{
        highScore = 0;
    }
    
    if(currentScore > highScore){
        writeHighScore(score);
        return true;
    }
    fclose(file);

    return false;
}

void terminarJogo(BulletVector *bullets, EnemyVector *enemies, Player *player){
    freeBulletVector(bullets);
    freeEnemyVector(enemies);
    sfSprite_destroy(player->playerSprite);
}

void rotateEnemy(Enemy *enemy, sfVector2f distance){
    float angle, angleDegress;
    angle = atan2(distance.y, distance.x);
    angleDegress = angle * 180 / PI;

    sfSprite_setRotation(enemy->shape, angleDegress);
}

void rotatePlayer(Player *player, sfVector2f distance){
    float angle, angleDegress;
    angle = atan2(distance.y, distance.x);
    angleDegress = angle * 180 / PI;

    sfSprite_setRotation(player->playerSprite, angleDegress);
}

void enemyFollowPlayer(Enemy *enemy, Player *player, float deltaTime, float speed){
    sfVector2f playerPos, enemyPos, distance;

    playerPos = sfSprite_getPosition(player->playerSprite);
    enemyPos = sfSprite_getPosition(enemy->shape);

    distance.x = playerPos.x - enemyPos.x;
    distance.y = playerPos.y - enemyPos.y;

    float length = sqrtf(distance.x * distance.x + distance.y * distance.y);
    if (length > 0) {
        distance.x /= length;
        distance.y /= length;
    }

    enemyPos.x += distance.x * speed * deltaTime * 60.f;
    enemyPos.y += distance.y * speed * deltaTime * 60.f;

    sfFloatRect playerBounds = sfSprite_getGlobalBounds(player->playerSprite);
    sfFloatRect enemyBounds = sfSprite_getGlobalBounds(enemy->shape);

    if(!sfFloatRect_intersects(&playerBounds, &enemyBounds, NULL)){
        rotateEnemy(enemy, distance);
        sfSprite_setPosition(enemy->shape, enemyPos);
    }
}

sfText *createText(sfFont *font, sfVector2f posicao, sfColor cor, int size){
    sfText *text = sfText_create();
    sfText_setPosition(text, posicao);
    sfText_setFillColor(text, cor);
    sfText_setCharacterSize(text, size);
    sfText_setFont(text, font);

    return text;
}

void getPlayerAim(sfRenderWindow *window, Player *player){
    sfVector2f pos = sfSprite_getPosition(player->playerSprite);
        
    player->playerCenterPos.x = pos.x;
    player->playerCenterPos.y = pos.y;

    sfVector2i pixelPos = sfMouse_getPositionRenderWindow(window);
    player->mousePos = sfRenderWindow_mapPixelToCoords(window, pixelPos, NULL);

    player->aimDirection.x = player->mousePos.x - player->playerCenterPos.x;
    player->aimDirection.y = player->mousePos.y - player->playerCenterPos.y;

    float length = sqrt(player->aimDirection.x * player->aimDirection.x + player->aimDirection.y * player->aimDirection.y);
    if (length != 0) {
        player->aimDirectionNormalize.x = player->aimDirection.x / length;
        player->aimDirectionNormalize.y = player->aimDirection.y / length;
    } 
}

void updateTimer(sfText *timer, sfClock *clock, char *tempoStr){
    int minutos, tempoEmSegundos;
    sfTime tempoPercorrido;

    tempoPercorrido = sfClock_getElapsedTime(clock);
    tempoEmSegundos = sfTime_asSeconds(tempoPercorrido);

    minutos = tempoEmSegundos / 60;
    tempoEmSegundos = tempoEmSegundos % 60;
    
    if(tempoEmSegundos < 10 && minutos < 10){
        sprintf(tempoStr, "0%d:0%d", minutos, tempoEmSegundos);
    }
    else if(tempoEmSegundos < 10){
        sprintf(tempoStr, "%d:0%d", minutos, tempoEmSegundos);
    }
    else if(minutos < 10){
        sprintf(tempoStr, "0%d:%d", minutos, tempoEmSegundos);
    }
    else{
        sprintf(tempoStr, "%d:%d", minutos, tempoEmSegundos);
    }
    
    sfText_setString(timer, tempoStr);
}

sfSprite *createSprite(int x, int y, sfVector2f tamanho, char *path){
    sfTexture *spriteImage = sfTexture_createFromFile(path, NULL);

    if(!spriteImage){
        printf("Imagem nao carregou");
        exit(1);
    } 

    sfSprite *sprite = sfSprite_create();

    sfSprite_setTexture(sprite, spriteImage, sfTrue);
    sfSprite_setScale(sprite, tamanho);
    sfSprite_setPosition(sprite, (sfVector2f){x, y});

    return sprite;
}


GameOver *menuInit(sfRenderWindow *window, sfFont *font, sfSprite *btnExit, sfSprite *btnRestart, sfSprite *bg){

    GameOver *menu = malloc(sizeof(GameOver));

    menu->selected = 0;

    menu->btnExit = btnExit;
    menu->btnRestart = btnRestart;
    menu->bg = bg;

    menu->text[0] = createText(font, (sfVector2f){405, 420}, sfWhite, 40);
    menu->text[1] = createText(font, (sfVector2f){663, 420}, sfWhite, 40);
    menu->textScore = createText(font, (sfVector2f){500, 510}, sfWhite, 50);
    menu->textTime = createText(font, (sfVector2f){460, 310}, sfWhite, 50);

    sfText_setString(menu->text[0], "RESTART");
    sfText_setString(menu->text[1], "EXIT");
        
    return menu;
}

void writeScoreGameOver(sfRenderWindow *window, GameOver *menu, int score){
    char textBuffer[20], scoreBuffer[10];
    itoa(score, scoreBuffer, 10);

    if(saveHighScore(scoreBuffer)){
        sfText_setPosition(menu->textScore, (sfVector2f){360, 510});
        sprintf(textBuffer, "NOVO HIGHSCORE: %d", score);
        sfText_setString(menu->textScore, textBuffer);
    }
    else{
        sprintf(textBuffer, "SCORE: %d", score);
        sfText_setString(menu->textScore, textBuffer);
    }
}

void drawMenu(sfRenderWindow *window, GameOver *menu){
    sfRenderWindow_drawSprite(window, menu->bg, NULL);
    sfRenderWindow_drawSprite(window, menu->btnRestart, NULL);
    sfRenderWindow_drawSprite(window, menu->btnExit, NULL);
    sfRenderWindow_drawText(window, menu->text[0], NULL);
    sfRenderWindow_drawText(window, menu->text[1], NULL);
    sfRenderWindow_drawText(window, menu->textScore, NULL);
    sfRenderWindow_drawText(window, menu->textTime, NULL);
}

int main() {
    sfVideoMode mode = {1200, 900, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "Top down shooter", sfResize | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 60);

    int enemySpawnTime = 0, shotCooldown = 0, score = 0, enemyCountMax = 5, vida = 100;
    int perdaVidaCooldown = 8, killCount = 0, spawmCooldown = 30, shotCooldownMax = 20, scoreMultiplier = 1;
    float speed = 2.5f;
    unsigned int enemyCount = 0;
    char scoreStr[64], scoreBuffer[10], vidaStr[3], tempoStr[6], timeGameOverBuffer[20];

    int jogoRodando = 1;

    sfClock *clock = sfClock_create();
    sfClock *clock2 = sfClock_create();

    if (!window)
        return 1;

    Player *player = createPlayer();
    
    sfFont *font = sfFont_createFromFile("./assets/fontes/Existence.ttf");
    if(!font)
        return 1;

    sfText *timer = createText(font, (sfVector2f){100, 800}, sfYellow, 60);
    sfText *scoreText = createText(font, (sfVector2f){1000, 20}, sfYellow, 34);
    sfText *playerLife = createText(font, (sfVector2f){100, 20}, sfYellow, 34);

    BulletVector *bullets = createBulletVector(10);
    EnemyVector *enemies = createEnemyVector(20);

    sfSprite *bg = createSprite(0, 0,(sfVector2f){1.2f, 1.2f}, "./assets/sprites/bg.jpeg");
    sfSprite *bgGameOver = createSprite(0, 0,(sfVector2f){1.5f, 1.5f}, "./assets/sprites/game_over.jpeg");
    sfSprite *btnExit = createSprite(390, 400,(sfVector2f){2.5f, 2.f}, "./assets/sprites/btn.png");
    sfSprite *btnRestart = createSprite(610, 400,(sfVector2f){2.5f, 2.f}, "./assets/sprites/btn.png");


    GameOver *menu = menuInit(window, font, btnExit, btnRestart, bgGameOver);
    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
            {
                terminarJogo(bullets, enemies, player);
            }
        }

        if(jogoRodando == 1){
        float deltaTime = sfTime_asSeconds(sfClock_restart(clock));
        updateTimer(timer, clock2, tempoStr);
        
        getPlayerAim(window, player);

        rotatePlayer(player, player->aimDirectionNormalize);

        if(sfKeyboard_isKeyPressed(sfKeyA) || sfKeyboard_isKeyPressed(sfKeyLeft)) sfSprite_move(player->playerSprite, (sfVector2f){-7.f, 0.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyD) || sfKeyboard_isKeyPressed(sfKeyRight)) sfSprite_move(player->playerSprite, (sfVector2f){7.f, 0.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyW) || sfKeyboard_isKeyPressed(sfKeyUp)) sfSprite_move(player->playerSprite, (sfVector2f){0.f, -7.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyS) || sfKeyboard_isKeyPressed(sfKeyDown)) sfSprite_move(player->playerSprite, (sfVector2f){0.f, 7.f});

        
        if(shotCooldown < shotCooldownMax) shotCooldown++;

        if(enemySpawnTime >= spawmCooldown && enemyCount < enemyCountMax){
            Enemy *newEnemy = createEnemy(player,1200, 900);
            pushBackEnemy(enemies, newEnemy);

            enemyCount++;
            enemySpawnTime = 0;
        }
        enemySpawnTime++;


        if(sfMouse_isButtonPressed(sfMouseLeft) && shotCooldown >= shotCooldownMax){
            Bullet *newBullet = createBullet(5);

            sfCircleShape_setPosition(newBullet->shape, player->playerCenterPos);
            newBullet->currentVelocity.x = player->aimDirectionNormalize.x * newBullet->maxSpeed;
            newBullet->currentVelocity.y = player->aimDirectionNormalize.y * newBullet->maxSpeed;

            pushBulletBack(bullets, newBullet);
            shotCooldown = 0;
        }
    
        sprintf(scoreStr, "SCORE: %d", score);
        sfText_setString(scoreText, scoreStr);

        sprintf(vidaStr, "Vida: %d", vida);
        sfText_setString(playerLife, vidaStr);

        for (int i = 0; i < bullets->size; i++) {

            sfCircleShape_move(bullets->bullets[i]->shape, bullets->bullets[i]->currentVelocity);

            sfVector2f pos = sfCircleShape_getPosition(bullets->bullets[i]->shape);
            float radius = sfCircleShape_getRadius(bullets->bullets[i]->shape);

            if (pos.x + radius < 0 || pos.x - radius > 1200 ||
                pos.y + radius < 0 || pos.y - radius > 900) {
                eraseBulletAtIndex(bullets, i);
                i--; 
                continue;
            }

            int collided = 0;
            for (int k = 0; k < enemies->size; k++) {
                sfFloatRect bulletBounds = sfCircleShape_getGlobalBounds(bullets->bullets[i]->shape);
                sfFloatRect enemyBounds = sfSprite_getGlobalBounds(enemies->enemies[k]->shape);
                
                if (sfFloatRect_intersects(&bulletBounds, &enemyBounds, NULL)) {
                    eraseBulletAtIndex(bullets, i);
                    eraseEnemyAtIndex(enemies, k);
                    i--; 
                    collided = 1;
                    enemyCount--;
                    score += 10 * scoreMultiplier;
                    enemySpawnTime = 0;
                    killCount++;
                    break;
                }
            }

            if (collided)
                continue;
        }
        
        for (int i = 0; i < enemies->size; i++){
            sfFloatRect enemyBounds = sfSprite_getGlobalBounds(enemies->enemies[i]->shape);

            sfFloatRect playerBounds = sfSprite_getGlobalBounds(player->playerSprite);

            if(sfFloatRect_intersects(&playerBounds, &enemyBounds, NULL)){
                perdaVidaCooldown--;
                if(perdaVidaCooldown == 0){
                    vida--;
                    perdaVidaCooldown = 10;
                }
            }
        }

        
        if(killCount == 5 && enemyCountMax <= 23){
            enemyCountMax++;
            spawmCooldown--;
            scoreMultiplier ++;
            speed += 0.1f;
            killCount = 0;
        }

        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawSprite(window, bg, NULL);
        sfRenderWindow_drawSprite(window, player->playerSprite, NULL); 
        

        for(int i = 0; i < enemies->size; i++) {
            sfRenderWindow_drawSprite(window, enemies->enemies[i]->shape, NULL);
            enemyFollowPlayer(enemies->enemies[i], player, deltaTime, speed);
        }
  
        for(int i = 0; i < bullets->size; i++) sfRenderWindow_drawCircleShape(window, bullets->bullets[i]->shape, NULL);
        sfRenderWindow_drawText(window, scoreText, NULL);
        sfRenderWindow_drawText(window, playerLife, NULL);
        sfRenderWindow_drawText(window, timer, NULL);
        
        
        sfRenderWindow_display(window); 
        if(vida <= 0){
            jogoRodando = 0;
            sprintf(timeGameOverBuffer, "TEMPO: %s", tempoStr);
            sfText_setString(menu->textTime, timeGameOverBuffer);

            terminarJogo(bullets, enemies, player);
            writeScoreGameOver(window, menu, score);
            }
        }
        else{
            sfRenderWindow_clear(window, sfBlack);

            drawMenu(window, menu);

             
            if (sfKeyboard_isKeyPressed(sfKeyRight) && menu->selected < 2 - 1) {
                sfText_setFillColor(menu->text[menu->selected], sfWhite);
                menu->selected++;
                sfText_setFillColor(menu->text[menu->selected], sfYellow);
            }
            if (sfKeyboard_isKeyPressed(sfKeyLeft) && menu->selected > 0) {
                sfText_setFillColor(menu->text[menu->selected], sfWhite);
                menu->selected--;
                sfText_setFillColor(menu->text[menu->selected], sfYellow);
            }
            if (sfKeyboard_isKeyPressed(sfKeyEnter)) {
                if(menu->selected == 0){
                    sfRenderWindow_close(window);
                    main();
                }
                if (menu->selected == 1)
                    sfRenderWindow_close(window);
                    sfRenderWindow_destroy(window);
            }
            

            sfRenderWindow_display(window); 
            if (event.type == sfEvtClosed)
            {
                sfRenderWindow_close(window);
                sfRenderWindow_destroy(window);
            }
        }
        
    }
    
    sfRenderWindow_destroy(window);

    return 0;
}