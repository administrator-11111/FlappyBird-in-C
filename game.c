/*
 * Flappy Bird in C
 * By Return
 * Version 1.0
 * Description: A simple console-based Flappy Bird game
 * Instructions: Use the arrow keys to control the bird. Avoid the pipes and try to get the highest score possible! (No finished yet / Add save system and more features)
 * I'm just trying to make it work for fun! :p
 * 4:59 PM 13/07
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/* INFORMATION*/
#define GAME_NAME "Flappy Bird in C"
#define GAME_AUTHOR "Return"
#define GAME_VERSION "1.0"
#define GAME_DESCRIPTION "A simples console-based Flappy Bird game."
#define GAME_INSTRUCTIONS "Use the arrow keys to control the bird. Avoid the pipes and try to get the highest score possible!"

/* SCREEN */
#define RES_WIDTH 32
#define RES_HEIGHT 16
#define RES_SIZE (RES_WIDTH * RES_HEIGHT) * 10

/* MAP */
#define MAX_PIPES 4
#define DISTANCE_BETWEEN_PIPES 8

/* COLOURS */
#define COLOUR_RESET    "\e[0m"
#define COLOUR_GREEN    "\e[32m"
#define COLOUR_YELLOW   "\e[33m"
#define COLOUR_RED      "\e[31m"
#define COLOUR_BLUE     "\e[34m"
#define COLOUR_BLACK    "\e[30m"

typedef struct Vector2 {
    int x;
    int y;
} vector2_t;

typedef struct Player {
    vector2_t position;
    int score;
    float health;
    int best_score;
} player_t;

typedef struct Pipe {
    vector2_t position;
    int gap_size;
} pipe_t;

typedef struct Map {
    vector2_t position;
    char screen[RES_SIZE];
} map_t;

typedef struct Game {
    struct Player player;
    struct Pipe pipe[MAX_PIPES];
    struct Map map;
} game_t;

/* Pipe declarations */
void create_pipe(game_t *, int, int, int, int);
void update_pipes(game_t *);

/* Screen declarations */
void initialize_screen(game_t *);
void initialize_game(game_t *);

void draw_info(game_t);
void update_screen(game_t *);
void draw(game_t);

/* Player declarations */
bool is_player_alive(player_t);
float set_player_health(player_t *, float);
int set_player_score(player_t *, int);
bool set_player_position(player_t *, int, int);
bool move_player(player_t *, int, int);
bool damage_player(player_t *, float);
bool heal_player(player_t *, float);
bool increase_player_score(player_t *, int);
bool decrease_player_score(player_t *, int);

/* Utils*/
int get_ansi_code_length(const char *);
void printmsg(const char *);
int random_range(int, int);

int main() {
    srand(time(NULL));

    game_t game;
    initialize_game(&game);
    while (1) {
        draw_info(game);
        update_pipes(&game);
        update_screen(&game);
        draw(game);
        move_player(&game.player, random_range(-1, 1), random_range(-1, 1));
    }
    return 0;
}

void create_pipe(game_t *game, int index, int x, int y, int gap_size) {
    if (index < 0 || index >= MAX_PIPES) {
        return;
    }
    game->pipe[index].position.x = x;
    game->pipe[index].position.y = y;
    game->pipe[index].gap_size = gap_size;
}

void update_pipes(game_t *game) {
    for (int i = 0; i < MAX_PIPES; i++) {
        game->pipe[i].position.x--;

        /* if the pipe has moved off the screen */
        if (game->pipe[i].position.x < 1) {
            int max_x = 0;
            for (int j = 0; j < MAX_PIPES; j++) {
                if (game->pipe[j].position.x > max_x) {
                    max_x = game->pipe[j].position.x;
                }
            }
            /* Create a new pipe at the right side */
            int new_x = max_x + DISTANCE_BETWEEN_PIPES;
            int new_y = (rand() % (RES_HEIGHT - 1)) + 1;
            create_pipe(game, i, new_x, new_y, random_range(1, 2));
        }
    }
}

void initialize_screen(game_t *game) {
    memset(game->map.screen, 0, sizeof(game->map.screen));
}

void initialize_game(game_t *game) {
    set_player_position(&game->player, RES_WIDTH / 4, RES_HEIGHT / 2);
    set_player_score(&game->player, 0);
    set_player_health(&game->player, 100.0f);
    game->map.position.x = 0;
    game->map.position.y = 0;

    initialize_screen(game);
    
    for (int i = 0; i < MAX_PIPES; i++) {
        create_pipe(game, i, RES_WIDTH + (i * DISTANCE_BETWEEN_PIPES), 5 + (rand() % 6), random_range(1, 2));
    }
}

void draw(game_t game) {
    printf("%s" COLOUR_RESET, game.map.screen);
    printf("\e[26A");
}

void update_screen(game_t *game) {
    initialize_screen(game);
    for (int y = 0; y <= RES_HEIGHT; y++) {   
        for (int x = 0; x <= RES_WIDTH; x++) {
            if (y == 0 || y == RES_HEIGHT || x == 0 || x == RES_WIDTH) {
                strcat(game->map.screen, COLOUR_YELLOW "\xFE ");
                continue;
            } 
            
            if (x == game->player.position.x && y == game->player.position.y) {
                strcat(game->map.screen, COLOUR_BLUE "\xFE ");
                continue;
            }
            

            bool is_pipe = false, is_debug_pipe = false;
            for (int i = 0; i < MAX_PIPES; i++) {
                if (game->pipe[i].position.x <= 0 || game->pipe[i].position.x >= RES_WIDTH) {
                    continue; 
                }
                if (y == game->pipe[i].position.y && x == game->pipe[i].position.x) {
                    is_debug_pipe = true;
                    break;
                }
                if (x >= game->pipe[i].position.x - 1 && x <= game->pipe[i].position.x + 1) {
                    if (y < game->pipe[i].position.y - game->pipe[i].gap_size || y > game->pipe[i].position.y + game->pipe[i].gap_size) {
                        is_pipe = true;
                        break;
                    }
                }
            }

            if (is_pipe) {
                strcat(game->map.screen, COLOUR_GREEN "\xFE ");
            } else if (is_debug_pipe) {
                strcat(game->map.screen, COLOUR_RED "\xFE ");
            } else {
                strcat(game->map.screen, COLOUR_BLACK "\xFE ");
            }
        }
        strcat(game->map.screen, "\n");
    }
}

void draw_info(game_t game) {
    char buffer[256];
    printmsg(COLOUR_YELLOW GAME_NAME " by " GAME_AUTHOR " - v" GAME_VERSION "\n" GAME_DESCRIPTION "\n" GAME_INSTRUCTIONS "\n\n");

    for (int i = 0; i < RES_WIDTH; i++) {
        printf("--");
    }
    printf("\n");
    printf("[SCORE: %d]  [HEALTH:"COLOUR_RED" %0.2f" COLOUR_RESET "]  " COLOUR_YELLOW "[DEBUG: (X: %02d, Y: %02d)] " COLOUR_RESET "\n", game.player.score, game.player.health, game.player.position.x, game.player.position.y);
    for (int i = 0; i < RES_WIDTH; i++) printf("--");
    printf("\n");
}

/*
    Player functions
*/

bool is_player_alive(player_t player) {
    return player.health > 0.0f;
}

float set_player_health(player_t *player, float health) {
    if (health < 0.0f) player->health = 0.0f;
    else if (health > 100.0f) player->health = 100.0f;
    else player->health = health;
    return player->health;
}

int set_player_score(player_t *player, int score) {
    if (score < 0) player->score = 0;
    else player->score = score;
    return player->score;
}

bool set_player_position(player_t *player, int x, int y) {
    if (x < 0 || x >= RES_WIDTH || y < 0 || y >= RES_HEIGHT) {
        return false;
    }
    player->position.x = x;
    player->position.y = y;
    return true;
}

bool move_player(player_t *player, int dx, int dy) {
    return set_player_position(player, player->position.x + dx, player->position.y + dy);
}

bool damage_player(player_t *player, float damage) {
    if (damage < 0.0f) return false;
    set_player_health(player, player->health - damage);
    return true;
}

bool heal_player(player_t *player, float heal) {
    if (heal < 0.0f) return false;
    set_player_health(player, player->health + heal);
    return true;
}

bool increase_player_score(player_t *player, int increment) {
    if (increment < 0) return false;
    set_player_score(player, player->score + increment);
    return true;
}

bool decrease_player_score(player_t *player, int decrement) {
    if (decrement < 0) return false;
    set_player_score(player, player->score - decrement);
    return true;
}

/* 
    Utils functions
*/
int random_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int get_ansi_code_length(const char *str) {
    if (*(str) == '\033' && *(str + 1) == '[') {
        int i = 2;
        while (*(str + i) != '\0' && *(str + i) != 'm') {
            i++;
        }
        if (*(str + i) == 'm') {
            return i + 1;
        }
    }
    return 0;
}

void printmsg(const char *msg) {
    int curr_idx = 0, visible_printed = 0;
    int max = RES_WIDTH * 2, ansi_len;

    while (*(msg + curr_idx) != '\0') {
        ansi_len = get_ansi_code_length(msg + curr_idx);
        
        if (ansi_len > 0) {
            printf("%.*s", ansi_len, msg + curr_idx);
            curr_idx += ansi_len;
        } 
        else if (*(msg + curr_idx) == '\n') {
            printf(COLOUR_RESET "\n"); 
            curr_idx++;
            visible_printed = 0;
        } 
        else {
            putchar(*(msg + curr_idx));
            curr_idx++;
            visible_printed++;

            if (visible_printed >= max) {
                printf(COLOUR_RESET "\n");
                visible_printed = 0;
            }
        }
    }
    printf(COLOUR_RESET); 
}