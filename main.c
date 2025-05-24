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

    player->playerSprite = sfCircleShape_create();
    sfCircleShape_setRadius(player->playerSprite, 25);
    sfCircleShape_setFillColor(player->playerSprite, sfColor_fromRGB(100, 250, 50));
    sfCircleShape_setPosition(player->playerSprite, (sfVector2f){600, 500});

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

void terminarJogo(sfRenderWindow *window, int score, char *scoreBuffer){
    sfRenderWindow_close(window);
    saveHighScore(itoa(score, scoreBuffer, sizeof(scoreBuffer)));
}


sfText *createScoreText(sfFont *font){
    sfText *scoreText= sfText_create();

    sfText_setFont(scoreText, font);
    sfText_setCharacterSize(scoreText, 20);
    sfText_setFillColor(scoreText, sfWhite);
    sfText_setPosition(scoreText, (sfVector2f){1000, 20});
    
    return scoreText;
}

sfText *createPlayerLifeText(sfFont *font){
    sfText *playerLife = sfText_create();

    sfText_setFont(playerLife, font);
    sfText_setCharacterSize(playerLife, 20);
    sfText_setFillColor(playerLife, sfWhite);
    sfText_setPosition(playerLife, (sfVector2f){100, 20});
    
    return playerLife;
}

void rotateEnemy(Enemy *enemy, sfVector2f distance){
    float angle, angleDegress;
    angle = atan2(distance.y, distance.x);
    angleDegress = angle * 180 / M_PI;

    sfSprite_setRotation(enemy->shape, angleDegress);
}

void enemyFollowPlayer(Enemy *enemy, Player *player, float deltaTime){
    sfVector2f playerPos, enemyPos, distance;
    

    playerPos = sfCircleShape_getPosition(player->playerSprite);
    enemyPos = sfSprite_getPosition(enemy->shape);

    distance.x= playerPos.x - enemyPos.x;
    distance.y = playerPos.y - enemyPos.y;

    float length = sqrtf(distance.x * distance.x + distance.y * distance.y);
    if (length > 0) {
        distance.x /= length;
        distance.y /= length;
    }

    enemyPos.x += distance.x * 3.5f * deltaTime * 60.f;
    enemyPos.y += distance.y * 3.5f * deltaTime * 60.f;

   
    rotateEnemy(enemy, distance);
    sfSprite_setPosition(enemy->shape, enemyPos);
    
}

int main() {
    sfVideoMode mode = {1200, 900, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "Janela CSFML", sfResize | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 60);

    int enemySpawnTime = 0, shotCooldown = 0, score = 0, enemyCountMax = 5, vida = 100, perdaVidaCooldown = 10;
    const int spawmCooldown = 30, shotCooldownMax = 20;
    unsigned int enemyCount = 0;
    char scoreStr[64], scoreBuffer[10], vidaStr[3];

    sfClock *clock = sfClock_create();

    if (!window)
        return 1;

    Player * player = createPlayer();

    sfFont *font = sfFont_createFromFile("./assets/fontes/Arial.ttf");
    if(!font)
        return 1;

    sfText *scoreText = createScoreText(font);
    sfText *playerLife = createPlayerLifeText(font);

    BulletVector *bullets = createBulletVector(10);
    EnemyVector *enemies = createEnemyVector(20);

    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
            {
                terminarJogo(window, score, scoreBuffer);
            }
        }
        float deltaTime = sfTime_asSeconds(sfClock_restart(clock));
        sfVector2f pos = sfCircleShape_getPosition(player->playerSprite);
        float radius = sfCircleShape_getRadius(player->playerSprite);
        
        player->playerCenterPos.x = pos.x + radius;
        player->playerCenterPos.y = pos.y + radius;

        sfVector2i pixelPos = sfMouse_getPositionRenderWindow(window);
        player->mousePos = sfRenderWindow_mapPixelToCoords(window, pixelPos, NULL);

        player->aimDirection.x = player->mousePos.x - player->playerCenterPos.x;
        player->aimDirection.y = player->mousePos.y - player->playerCenterPos.y;

       float length = sqrt(player->aimDirection.x * player->aimDirection.x + player->aimDirection.y * player->aimDirection.y);
        if (length != 0) {
            player->aimDirectionNormalize.x = player->aimDirection.x / length;
            player->aimDirectionNormalize.y = player->aimDirection.y / length;
        } else {
            player->aimDirectionNormalize.x = 0;
            player->aimDirectionNormalize.y = 0;
        }
        

        if(sfKeyboard_isKeyPressed(sfKeyA) || sfKeyboard_isKeyPressed(sfKeyLeft)) sfCircleShape_move(player->playerSprite, (sfVector2f){-10.f, 0.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyD) || sfKeyboard_isKeyPressed(sfKeyRight)) sfCircleShape_move(player->playerSprite, (sfVector2f){10.f, 0.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyW) || sfKeyboard_isKeyPressed(sfKeyUp)) sfCircleShape_move(player->playerSprite, (sfVector2f){0.f, -10.f});
        
        if(sfKeyboard_isKeyPressed(sfKeyS) || sfKeyboard_isKeyPressed(sfKeyDown)) sfCircleShape_move(player->playerSprite, (sfVector2f){0.f, 10.f});

        
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
                
                sfFloatRect playerBounds = sfCircleShape_getGlobalBounds(player->playerSprite);
                
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
        
        for (int i = 0; i < enemies->size; i++){
            sfFloatRect enemyBounds = sfSprite_getGlobalBounds(enemies->enemies[i]->shape);

            sfFloatRect playerBounds = sfCircleShape_getGlobalBounds(player->playerSprite);

            if(sfFloatRect_intersects(&playerBounds, &enemyBounds, NULL)){
                perdaVidaCooldown--;
                if(perdaVidaCooldown == 0){
                    vida--;
                    perdaVidaCooldown = 10;
                    printf("Vida: %d\n", vida);
                }
            }
        }

        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawCircleShape(window, player->playerSprite, NULL); 

        for(int i = 0; i < enemies->size; i++) {
            sfRenderWindow_drawSprite(window, enemies->enemies[i]->shape, NULL);
            enemyFollowPlayer(enemies->enemies[i], player, deltaTime);
            
        }
  
        for(int i = 0; i < bullets->size; i++) sfRenderWindow_drawCircleShape(window, bullets->bullets[i]->shape, NULL);
        sfRenderWindow_drawText(window, scoreText, NULL);
        sfRenderWindow_drawText(window, playerLife, NULL);
            
        sfRenderWindow_display(window); 
        if(vida <= 0) terminarJogo(window, score, scoreBuffer);
    }

    freeBulletVector(bullets);
    freeEnemyVector(enemies);
    sfCircleShape_destroy(player->playerSprite);
    sfRenderWindow_destroy(window);

    return 0;

}