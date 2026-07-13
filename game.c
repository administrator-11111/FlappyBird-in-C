/*
 * Flappy Bird in C
 * By Return
 * Version 1.0
 * Description: A simple console-based Flappy Bird game
 * Instructions: Use the arrow keys to control the bird. Avoid the pipes and try to get the highest score possible! (No finished yet / Add save system and more features)
 * I'm just trying to make it work for fun! :p
 * 13:21 PM 13/07
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
#endif

/* INFORMATION*/
#define GAME_NAME "Flappy Bird in C"
#define GAME_AUTHOR "Return"
#define GAME_VERSION "1.0"
#define GAME_DESCRIPTION "A simples console-based Flappy Bird game."
#define GAME_INSTRUCTIONS "Use the arrow keys to control the bird. Avoid the pipes and try to get the highest score possible!"

/* GAME SETTINGS */
#define TARGET_FPS 120
#define MAX_PLAYER_NAME 24
/* SCREEN */
#define RES_WIDTH 64
#define RES_HEIGHT 32
#define RES_SIZE ((RES_WIDTH + 1) * (RES_HEIGHT + 1)) * 15

/* PLAYER SIZE */
#define PLAYER_HEIGHT (RES_HEIGHT / 12 < 1 ? 1 : RES_HEIGHT / 12)
#define PLAYER_WIDTH  (PLAYER_HEIGHT)

/* MAP */
#define DISTANCE_BETWEEN_PIPES (RES_WIDTH / 4 < 5 ? 8 : RES_WIDTH / 4)
#define MAX_PIPES ((RES_WIDTH / DISTANCE_BETWEEN_PIPES) + 2)

#define MIN_GAP (PLAYER_HEIGHT + 1)
#define MAX_GAP (MIN_GAP + (RES_HEIGHT / 8))

/* COLOURS */
#define COLOUR_RESET    "\e[0m"
#define COLOUR_GREEN    "\e[32m"
#define COLOUR_YELLOW   "\e[33m"
#define COLOUR_YELLOW_L   "\e[93m"
#define COLOUR_RED      "\e[31m"
#define COLOUR_BLUE_L    "\e[96m"
#define COLOUR_BLUE     "\e[34m"
#define COLOUR_BLACK    "\e[30m"
#define COLOUR_ORANGE   "\033[38;5;208m"
#define COLOUR_GREY     "\e[90m"
#define COLOUR_WHITE    "\033[1;37m"

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

typedef struct Settings {
    char username[MAX_PLAYER_NAME];
    int width;
    int height;
    int pipe_distance;
    int gapes_range;
} settings_t;

typedef struct Game {
    struct Player player;
    struct Pipe pipe[MAX_PIPES];
    struct Settings settings;
    char screen[RES_SIZE];
    int current_fps;
} game_t;

/* Settings declarations */

bool load_settings(game_t *);
bool save_settings(game_t *);

/* Pipe declarations */
void create_pipe(game_t *, int, int, int, int);
void update_pipes(game_t *);

/* Screen declarations */
void initialize_screen(game_t *);
void initialize_game(game_t *);

int colision(game_t *);

void draw_stats(game_t);
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
    setvbuf(stdout, NULL, _IOFBF, 0);
    static game_t game;
    initialize_game(&game);

    draw_info(game);
    time_t last_time = time(NULL), curr_time;
    int frame_count = 0;
    while (1) {
        curr_time = time(NULL);
        if (curr_time != last_time) {
            game.current_fps = frame_count;
            frame_count = 0;
            last_time = curr_time;
        }
        frame_count++;

        draw_stats(game);
        update_pipes(&game);
        update_screen(&game);
        draw(game);
        move_player(&game.player, random_range(-1, 1), random_range(-1, 1));
        
        #if defined(_WIN32) || defined(_WIN64)
            Sleep(1000 / TARGET_FPS); 
        #else
            usleep(1000000 / TARGET_FPS); 
        #endif
    }
    return 0;
}

void default_settings(settings_t * settings) {
    settings->username[0] = '\0';
    settings->height = RES_HEIGHT;
    settings->width = RES_WIDTH;
    settings->pipe_distance = 8;
    settings->gapes_range = 2; 
}

bool load_settings(game_t *game, const char *s_file) {
    FILE *f = fopen(s_file, "r");
    if (f == NULL) {
        default_settings(&game->settings);
        return false;
    }

    char line[64];
    char key[32];
    int value;

    default_settings(&game->settings);

    while (fgets(line, sizeof(line), f) != NULL) {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '#') {
            continue;
        }

        if (sscanf(line, "%63[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "game.width") == 0) {
                game->settings.width = value;
            } 
            else if (strcmp(key, "game.height") == 0) {
                game->settings.height = value;
            }
            else if (strcmp(key, "game.fps") == 0) {
                game->settings.fpslimit = value;
            }
            else if (strcmp(key, "map.pipes_distance") == 0) {
                game->settings.pipe_distance = value;
            }
            else if (strcmp(key, "map.gapes_range") == 0) {
                game->settings.gapes_range = value;
            }
            else if (strcmp(key, "user.name") == 0) {
                game->settings.username = value;
            }
            
        }
    }

    fclose(f);
    return true;
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
            int 
                new_x = max_x + DISTANCE_BETWEEN_PIPES,
                new_y = random_range(MAX_GAP + 2, RES_HEIGHT - MAX_GAP - 2),
                range = random_range(MIN_GAP, MAX_GAP);
        
            create_pipe(game, i, new_x, new_y, range);
        }
    }
}

void initialize_screen(game_t *game) {
    memset(game->screen, 0, sizeof(game->screen));
}

void initialize_game(game_t *game) {
    set_player_position(&game->player, RES_WIDTH / 4, RES_HEIGHT / 2);
    set_player_score(&game->player, 0);
    set_player_health(&game->player, 100.0f);
    initialize_screen(game);
    
    for (int i = 0; i < MAX_PIPES; i++) {
        create_pipe(game, i, RES_WIDTH + (i * DISTANCE_BETWEEN_PIPES), random_range(MAX_GAP + 2, RES_HEIGHT - MAX_GAP - 2), random_range(MIN_GAP, MAX_GAP));
    }
}

void draw(game_t game) {
    fputs(game.screen, stdout);
    fputs(COLOUR_RESET, stdout);

    printf("\e[%dA", RES_HEIGHT + 4);
    fflush(stdout);

}

void update_screen(game_t *game) {
    initialize_screen(game);
    
    int offset = 0; 

    for (int y = 0; y <= RES_HEIGHT; y++) {   
        for (int x = 0; x <= RES_WIDTH; x++) {
            if (y == 0 || y == RES_HEIGHT || x == 0 || x == RES_WIDTH) {
                offset += sprintf(game->screen + offset, "%s", COLOUR_YELLOW "\xFE ");
                continue;
            }
            /* Player draw */
            if (y >= game->player.position.y && y < game->player.position.y + PLAYER_HEIGHT && x >= game->player.position.x && x < game->player.position.x + PLAYER_WIDTH) {
                offset += sprintf(game->screen + offset, "%s", COLOUR_YELLOW_L "\xFE");

                if (x == game->player.position.x + PLAYER_WIDTH - 1) {
                    offset += sprintf(game->screen + offset, "%s", COLOUR_RED "\xAF ");
                }
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
                offset += sprintf(game->screen + offset, "%s", COLOUR_GREEN "\xFE ");
            } else if (is_debug_pipe) {
                offset += sprintf(game->screen + offset, "%s", COLOUR_RED "\xFE ");
            } else {
                offset += sprintf(game->screen + offset, "%s", COLOUR_BLACK "\xFE ");
            }
        }
        offset += sprintf(game->screen + offset, "\n");
    }
}

void draw_stats(game_t game) {
    for (int i = 0; i < RES_WIDTH; i++) {
        printf("--");
    }
    printf("\n");
    printf(COLOUR_WHITE "[FPS:" COLOUR_YELLOW " %d" COLOUR_WHITE "]  [SCORE: " COLOUR_YELLOW_L "%d" COLOUR_WHITE "]  [HEALTH:"COLOUR_RED" %0.2f" COLOUR_WHITE "]  " COLOUR_GREY "[DEBUG: (X: %02d, Y: %02d)] " COLOUR_RESET "\n", game.current_fps, game.player.score, game.player.health, game.player.position.x, game.player.position.y);
    for (int i = 0; i < RES_WIDTH; i++) printf("--");
    printf("\n");
}

void draw_info(game_t game) {
    printmsg(COLOUR_YELLOW GAME_NAME " by " GAME_AUTHOR " - v" GAME_VERSION "\n" GAME_DESCRIPTION "\n" GAME_INSTRUCTIONS "\n\n");
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