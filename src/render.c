#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "render.h"
#include "game.h"
#include "utils.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#ifndef VK_UP
#define VK_UP 0x26
#define VK_DOWN 0x28
#define VK_RETURN 0x0D
#define VK_BACK 0x08
#define VK_ESCAPE 0x1B
#endif

int fast_append(char *dest, const char *src) {
    int len = 0;
    while (*src) {
        *dest++ = *src++;
        len++;
    }
    return len;
}

int get_buffer_size(game_t game) {
    return (((game.settings.resolution.x + 1) * (game.settings.resolution.y + 1)) * 30);
}

bool allocate_screen_memory(game_t * game) {
    int buffer_size = get_buffer_size(*game);
    game->screen = (char *)malloc(buffer_size * sizeof(char));
    if (game->screen == NULL) return false;
    memset(game->screen, 0, buffer_size);
    return true;
}

void cleanup_memory(game_t *game) {
    if (game->screen != NULL) {
        free(game->screen);
        game->screen = NULL;
    }
}

void initialize_screen(game_t *game) {
    int buffer_size = get_buffer_size(*game);
    memset(game->screen, 0, buffer_size);
}

void draw(game_t game) {
    fputs(game.screen, stdout);
    fputs(COLOUR_RESET, stdout);
    fflush(stdout);
}

void update_screen(game_t *game) {
    initialize_screen(game);

    int offset = 0;
    int height = game->settings.resolution.y;
    int width = game->settings.resolution.x;
    
    int p_height = get_game_height(game);
    int p_width = get_game_width(game);
    int active_pipes = get_active_pipes(game);

    for (int y = 0; y <= height; y++) {   
        for (int x = 0; x <= width; x++) {
            
            if (y == 0 || y == height || x == 0 || x == width) {
                offset += fast_append(game->screen + offset, COLOUR_YELLOW "\xFE ");
                continue;
            }

            if (y >= game->player.position.y && y < game->player.position.y + p_height && 
                x >= game->player.position.x && x < game->player.position.x + p_width) {
                
                if (x == game->player.position.x + p_width - 1) {
                    offset += fast_append(game->screen + offset, COLOUR_RED "\xAF "); 
                } else {
                    if (game->player.velocity_y < -0.2f) {
                        offset += fast_append(game->screen + offset, COLOUR_YELLOW_L "\x5E ");
                    } else if (game->player.velocity_y > 0.4f) {
                        offset += fast_append(game->screen + offset, COLOUR_YELLOW_L "v ");
                    } else {
                        offset += fast_append(game->screen + offset, COLOUR_YELLOW_L "\xFE ");
                    }
                }
                continue;
            }

            bool is_pipe = false, is_debug_pipe = false;
            for (int i = 0; i < active_pipes; i++) {
                if (game->pipe[i].position.x <= 0 || game->pipe[i].position.x >= width) continue; 
                
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
                offset += fast_append(game->screen + offset, COLOUR_GREEN "\xFE ");
            } else if (is_debug_pipe) {
                offset += fast_append(game->screen + offset, COLOUR_RED "\xFE ");
            } else {
                offset += fast_append(game->screen + offset, COLOUR_BLACK "  "); 
            }
        }
        offset += fast_append(game->screen + offset, "\n");
    }
}

void draw_stats(game_t game) {
    int i;
    for (i = 0; i < game.settings.resolution.x; i++) printf("--");
    printf("\n");
    printf(COLOUR_WHITE "[PLAYER: %s]  [FPS:" COLOUR_YELLOW " %02d" COLOUR_WHITE "]  [SCORE: " COLOUR_YELLOW_L "%02d" COLOUR_WHITE "]  [BEST:"COLOUR_ORANGE" %02d" COLOUR_WHITE "]  " COLOUR_GREY "[DEBUG: (X: %02d, Y: %02d)] " COLOUR_RESET "\n", game.settings.user.nickname, game.current_fps, game.player.score, game.settings.user.best_score, game.player.position.x, game.player.position.y);
    for (i = 0; i < game.settings.resolution.x; i++) printf("--");
    printf("\n");
}

void draw_info(game_t game) {
    printmsg(COLOUR_YELLOW "Flappy Bird in C by Return - v1.0\nA simples console-based Flappy Bird game.\nUse SPACE or UP ARROW to fly.\n\n");
}

int ShowDialog(game_t *game, const char *title, const char *desc, const char **options, int option_count) {
    int selected = 0;
    bool key_pressed = false;

    /* clear screen */
    printf("\e[1;1H\e[2J"); 

    while (1) {
        #if defined(_WIN32) || defined(_WIN64)
        if (GetAsyncKeyState(VK_UP) & 0x8000) {
            if (!key_pressed) { selected--; key_pressed = true; }
        } else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
            if (!key_pressed) { 
                selected++; 
                key_pressed = true; 
            }
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            while (GetAsyncKeyState(VK_RETURN) & 0x8000) { 
                Sleep(10); 
            } 
            return selected;
        } else if (GetAsyncKeyState(VK_BACK) & 0x8000 || GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            Sleep(200);
            return -1;
        } else {
            key_pressed = false;
        }
        #endif

        if (selected < 0) selected = option_count - 1;
        if (selected >= option_count) selected = 0;

        initialize_screen(game);
        int height = game->settings.resolution.y;
        int width = game->settings.resolution.x;
        int inner_chars = (width - 1) * 2;
        int offset = 0;

        for (int y = 0; y <= height; y++) {
            if (y == 0 || y == height) {
                for (int x = 0; x <= width; x++) 
                    offset += fast_append(game->screen + offset, COLOUR_YELLOW "\xFE ");
            } else {
                offset += fast_append(game->screen + offset, COLOUR_YELLOW "\xFE " COLOUR_RESET);
                
                char line_buf[256] = "";
                int content_len = 0;

                if (y == height/4) { 
                    sprintf(line_buf, COLOUR_BLUE_L "%s" COLOUR_RESET, title);
                    content_len = strlen(title);
                } else if (y == height/4 + 2) { 
                    sprintf(line_buf, COLOUR_GREY "%s" COLOUR_RESET, desc);
                    content_len = strlen(desc);
                } else if (y >= height/2 && y < height/2 + option_count) { 
                    int opt_idx = y - height/2;
                    char temp[64];
                    if (opt_idx == selected) sprintf(temp, "> %s <", options[opt_idx]);
                    else sprintf(temp, "  %s  ", options[opt_idx]);
                    
                    sprintf(line_buf, COLOUR_WHITE "%s" COLOUR_RESET, temp);
                    content_len = strlen(temp);
                }

                if (content_len > 0) {
                    int pad_left = (inner_chars - content_len) / 2;
                    int pad_right = inner_chars - content_len - pad_left;
                    offset += sprintf(game->screen + offset, "%*s%s%*s", pad_left, "", line_buf, pad_right, "");
                } else {
                    offset += sprintf(game->screen + offset, "%*s", inner_chars, "");
                }
                offset += fast_append(game->screen + offset, COLOUR_YELLOW "\xFE ");
            }
            offset += fast_append(game->screen + offset, "\n");
        }

        printf("\e[H"); 
        draw(*game);

        #if defined(_WIN32) || defined(_WIN64)
            Sleep(1000 / TARGET_FPS); 
        #else
            usleep(1000000 / TARGET_FPS); 
        #endif
    }
}