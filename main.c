#include <SFML/Graphics.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


typedef struct
{
    sfCircleShape* playerSprite;
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

typedef struct {
    int capacity;
    int size;
    sfRectangleShape **enemies;
}EnemyVector;

EnemyVector* createEnemyVector(int capacity) {
    EnemyVector* vec = malloc(sizeof(EnemyVector));
    vec->enemies = malloc(sizeof(sfRectangleShape*) * capacity);
    vec->size = 0;
    vec->capacity = capacity;
    return vec;
}

sfRectangleShape* createEnemy(int windowWidth, int windowHeight) {
    sfRectangleShape* enemy = sfRectangleShape_create();
    sfRectangleShape_setSize(enemy, (sfVector2f){40, 40});
    sfRectangleShape_setFillColor(enemy, sfColor_fromRGB(255, 100, 100));

    float x = rand() % (windowWidth - 40);
    float y = rand() % (windowHeight - 40);
    sfRectangleShape_setPosition(enemy, (sfVector2f){x, y});

    return enemy;
}

void pushBackEnemy(EnemyVector* vector, sfRectangleShape* enemy) {
    if (vector->size >= vector->capacity) {
        vector->capacity *= 2;
        vector->enemies = realloc(vector->enemies, sizeof(sfRectangleShape*) * vector->capacity);
    }
    vector->enemies[vector->size++] = enemy;
}

void eraseEnemyAtIndex(EnemyVector* vector, int index) {
    if (index < 0 || index >= vector->size) return;

    sfRectangleShape_destroy(vector->enemies[index]);

    for (int i = index; i < vector->size - 1; i++) {
        vector->enemies[i] = vector->enemies[i + 1];
    }

    vector->size--;
}


void freeEnemyVector(EnemyVector* vector) {
    for (int i = 0; i < vector->size; i++) {
        sfRectangleShape_destroy(vector->enemies[i]);
    }
    free(vector->enemies);
    free(vector);
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
        fprintf(file, score);
        fclose(file);
    }
}

void saveHighScore(char *score){
    char textBuffer[10];
    FILE *file = fopen("Highscore.txt", "r");

    if(file == NULL){
        writeHighScore(score);
        return;
    }

    while (fgets(textBuffer, sizeof(textBuffer), file) != NULL){
        if(atoi(score) > atoi(textBuffer)){
            writeHighScore(score);
        }
    } 
    
    fclose(file);
}

int main() {
    sfVideoMode mode = {1200, 900, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "Janela CSFML", sfResize | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 60);

    int enemySpawnTime = 0, shotCooldown = 0, score = 0;
    const int spawmCooldown = 30, shotCooldownMax = 20, enemyCountMax = 5;
    unsigned int enemyCount = 0;
    char scoreStr[64], scoreBuffer[10];

    if (!window)
        return 1;

    Player player;


    player.playerSprite = sfCircleShape_create();
    sfCircleShape_setRadius(player.playerSprite, 25);
    sfCircleShape_setFillColor(player.playerSprite, sfColor_fromRGB(100, 250, 50));
    sfCircleShape_setPosition(player.playerSprite, (sfVector2f){375, 275});

    sfFont *font = sfFont_createFromFile("./assets/fontes/Arial.ttf");
    if(!font)
        return 1;

    sfText *scoreText = sfText_create();
    sfText_setFont(scoreText, font);
    sfText_setCharacterSize(scoreText, 20);
    sfText_setFillColor(scoreText, sfWhite);
    sfText_setPosition(scoreText, (sfVector2f){1000, 20});

    BulletVector *bullets = createBulletVector(10);
    EnemyVector *enemies = createEnemyVector(20);

    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed ||
               (event.type == sfEvtKeyPressed && event.key.code == sfKeyEscape))
            {
                sfRenderWindow_close(window);
                saveHighScore(itoa(score, scoreBuffer, sizeof(scoreBuffer)));
            }
        }
        sfVector2f pos = sfCircleShape_getPosition(player.playerSprite);
        float radius = sfCircleShape_getRadius(player.playerSprite);

        player.playerCenterPos.x = pos.x + radius;
        player.playerCenterPos.y = pos.y + radius;

        sfVector2i pixelPos = sfMouse_getPositionRenderWindow(window);
        player.mousePos = sfRenderWindow_mapPixelToCoords(window, pixelPos, NULL);

        player.aimDirection.x = player.mousePos.x - player.playerCenterPos.x;
        player.aimDirection.y = player.mousePos.y - player.playerCenterPos.y;

       float length = sqrt(player.aimDirection.x * player.aimDirection.x + player.aimDirection.y * player.aimDirection.y);
        if (length != 0) {
            player.aimDirectionNormalize.x = player.aimDirection.x / length;
            player.aimDirectionNormalize.y = player.aimDirection.y / length;
        } else {
            player.aimDirectionNormalize.x = 0;
            player.aimDirectionNormalize.y = 0;
        }
        

        if(sfKeyboard_isKeyPressed(sfKeyA)) sfCircleShape_move(player.playerSprite, (sfVector2f){-10.f, 0.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyD)) sfCircleShape_move(player.playerSprite, (sfVector2f){10.f, 0.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyW)) sfCircleShape_move(player.playerSprite, (sfVector2f){0.f, -10.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyS)) sfCircleShape_move(player.playerSprite, (sfVector2f){0.f, 10.f});
        
        if(shotCooldown < shotCooldownMax) shotCooldown++;

        if(enemySpawnTime >= spawmCooldown && enemyCount < enemyCountMax){
            sfRectangleShape *newEnemy = createEnemy(1200, 900);
            pushBackEnemy(enemies, newEnemy);

            enemyCount++;
            enemySpawnTime = 0;
        }
        enemySpawnTime++;


        if(sfMouse_isButtonPressed(sfMouseLeft) && shotCooldown >= shotCooldownMax){
            Bullet *newBullet = createBullet(5);

            sfCircleShape_setPosition(newBullet->shape, player.playerCenterPos);
            newBullet->currentVelocity.x = player.aimDirectionNormalize.x * newBullet->maxSpeed;
            newBullet->currentVelocity.y = player.aimDirectionNormalize.y * newBullet->maxSpeed;

            pushBulletBack(bullets, newBullet);
            shotCooldown = 0;
        }
    
        sprintf(scoreStr, "SCORE: %d", score);
        sfText_setString(scoreText, scoreStr);
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
                sfFloatRect enemyBounds = sfRectangleShape_getGlobalBounds(enemies->enemies[k]);

                if (sfFloatRect_intersects(&bulletBounds, &enemyBounds, NULL)) {
                    eraseBulletAtIndex(bullets, i);
                    eraseEnemyAtIndex(enemies, k);
                    i--; 
                    collided = 1;
                    enemyCount--;
                    score += 10;
                    enemySpawnTime = 0;
                    break;
                }
            }

            if (collided)
                continue;
        }
        
        

        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawCircleShape(window, player.playerSprite, NULL); 

        for(int i = 0; i < enemies->size; i++) sfRenderWindow_drawRectangleShape(window, enemies->enemies[i], NULL);
        for(int i = 0; i < bullets->size; i++)sfRenderWindow_drawCircleShape(window, bullets->bullets[i]->shape, NULL);
        sfRenderWindow_drawText(window, scoreText, NULL);
            

        sfRenderWindow_display(window); 
    }

    freeBulletVector(bullets);
    freeEnemyVector(enemies);
    sfCircleShape_destroy(player.playerSprite);
    sfRenderWindow_destroy(window);

    return 0;

}
