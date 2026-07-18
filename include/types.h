#ifndef TYPES_H
#define TYPES_H

/* DEFAULT SETTINGS */
#define TARGET_FPS 60
#define MAX_PLAYER_NAME 24
#define RES_WIDTH 64
#define RES_HEIGHT 32

/* MAP */
#define MAX_PIPES (50)

/* PHYSICS */
#define GRAVITY 0.1f
#define FLAP_FORCE -1.2f

/* KEYS */
#define VK_R (0x52)

/* COLOURS */
#define COLOUR_RESET        "\e[0m"
#define COLOUR_GREEN        "\e[32m"
#define COLOUR_YELLOW       "\e[33m"
#define COLOUR_YELLOW_L     "\e[93m"
#define COLOUR_RED          "\e[31m"
#define COLOUR_BLUE_L       "\e[96m"
#define COLOUR_BLUE         "\e[34m"
#define COLOUR_BLACK        "\e[30m"
#define COLOUR_ORANGE       "\033[38;5;208m"
#define COLOUR_GREY         "\e[90m"
#define COLOUR_WHITE        "\033[1;37m"

typedef enum {
    STATE_MENU,
    STATE_SETTINGS,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_EXIT
} GameState;

typedef struct Vector2 {
    int x;
    int y;
} vector2_t;

typedef struct Player {
    char nickname[MAX_PLAYER_NAME];
    vector2_t position;
    float exact_y;
    float velocity_y;
    int score;
    float health;
    int best_score;
} player_t;

typedef struct Pipe {
    vector2_t position;
    int gap_size;
} pipe_t;

typedef struct Settings {
    player_t user;
    vector2_t resolution;
    int pipe_distance;
    int gapes_range;
    int fpslimit;
} settings_t;

typedef struct Game {
    struct Player player;
    struct Pipe pipe[MAX_PIPES];
    struct Settings settings;
    char *screen;
    int current_fps;
} game_t;

#endif