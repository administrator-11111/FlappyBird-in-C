#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "types.h"
#include "settings.h"
#include "game.h"
#include "render.h"
#include "utils.h"

#define GAME_NAME "Flappy Bird in C"
#define GAME_AUTHOR "Return"
#define GAME_VERSION "1.0"
#define GAME_DESCRIPTION "A simples console-based Flappy Bird game."
#define GAME_INSTRUCTIONS "Use the UP arrow key to control the bird. Avoid the pipes and try to get the highest score possible!"

#define SETTINGS_FILENAME "settings.cfg"

int main() {
    srand(time(NULL));
    setvbuf(stdout, NULL, _IOFBF, 0);
    
    static game_t game;

    if (!load_settings(&game, SETTINGS_FILENAME)) {
        save_settings(&game, SETTINGS_FILENAME);
    }

    if (!allocate_screen_memory(&game)) {
        printf(COLOUR_RED "No se logró reservar memoria con la resolución actual (%d x %d)" COLOUR_RESET "\n", game.settings.resolution.x, game.settings.resolution.y);
        return 0;
    }

    const char *main_menu_options[] = {"Start Game", "Settings", "Quit"};
    const char *settings_menu_options[] = {"Back"};
    const char *game_over_options[] = {"Play Again", "Main Menu"};

    GameState curr_state = STATE_MENU;

    while (curr_state != STATE_EXIT) {
        switch (curr_state) {
            /* main screen */
            case STATE_MENU: {
                int choice = ShowDialog(&game, GAME_NAME, "Use UP/DOWN to navigate, ENTER to select", main_menu_options, 3);
                if (choice == 0) {
                    curr_state = STATE_PLAYING;
                } else if (choice == 1) {
                    curr_state = STATE_SETTINGS;
                } else {
                    curr_state = STATE_EXIT;
                }
                break;
            }
            case STATE_SETTINGS: {
                char opt_name[64], opt_res[64], opt_fps[64], opt_pipe[64], opt_gap[64], opt_reset[64], opt_back[64];
                
                sprintf(opt_name, "Nickname: %s", game.settings.user.nickname);
                sprintf(opt_res, "Resolution: %dx%d", game.settings.resolution.x, game.settings.resolution.y);
                sprintf(opt_fps, "Max FPS: %d", game.settings.fpslimit);
                sprintf(opt_pipe, "Pipes Distance: %d", game.settings.pipe_distance);
                sprintf(opt_gap, "Max Gape Size: %d", game.settings.gapes_range);
                sprintf(opt_reset, "Restart Record (Current: %d)", game.settings.user.best_score);
                sprintf(opt_back, "Back to menu");

                const char *settings_options[] = {opt_name, opt_res, opt_fps, opt_pipe, opt_gap, opt_reset, opt_back};
                
                int choice = ShowDialog(&game, "SETTINGS", "Select an option to modify", settings_options, 7);
                
                if (choice == 0) {
                    #if defined(_WIN32) || defined(_WIN64)
                        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
                    #endif
                    printf("\e[1;1H\e[2J"); 
                    printf(COLOUR_YELLOW "\n=== MODIFY NICKNAME ===\n\n" COLOUR_RESET);
                    printf("New nickname (no spaces): ");
                    
                    char input_buffer[64];
                    if (fgets(input_buffer, sizeof(input_buffer), stdin)) {
                        sscanf(input_buffer, "%23s", game.settings.user.nickname);
                    }
                    save_settings(&game, SETTINGS_FILENAME);

                    #if defined(_WIN32) || defined(_WIN64)
                        while (GetAsyncKeyState(VK_RETURN) & 0x8000) { 
                            Sleep(10); 
                        }
                    #endif
                } 
                else if (choice == 1) {
                    const char *res_opt_str[] = {"32x16 (Retro)", "64x32 (Classic)", "120x60 (Large)"};
                    const int res_opts[3][2] = { {32, 16}, {64, 32}, {120, 60}}; 
                    
                    int res_choice = ShowDialog(&game, "RESOLUTION", "Select screen size", res_opt_str, 3);
                    
                    if (res_choice >= 0 && res_choice < 3) {
                        game.settings.resolution.x = res_opts[res_choice][0]; 
                        game.settings.resolution.y = res_opts[res_choice][1];

                        cleanup_memory(&game);
                        if (!allocate_screen_memory(&game)) return 1; 
                        save_settings(&game, SETTINGS_FILENAME);
                    }
                } 
                else if (choice == 2) {
                    const char *fps_opts[] = {"30 FPS", "60 FPS", "90 FPS", "120 FPS"};
                    const int fps_options[4]= {30, 60, 90, 120}; 
                    int fps_choice = ShowDialog(&game, "MAX FPS", "Select target framerate", fps_opts, 4);
                    
                    if (fps_choice != -1 && fps_choice < 4) {
                        game.settings.fpslimit = fps_options[fps_choice];
                        save_settings(&game, SETTINGS_FILENAME);
                    }
                } 
                else if (choice == 3) {
                    const char *pipe_opts[] = {"8 (Hard)", "12 (Normal)", "16 (Easy)", "20 (Very Easy)"};
                    int pipe_choice = ShowDialog(&game, "PIPES DISTANCE", "Select distance between pipes", pipe_opts, 4);
                    
                    if (pipe_choice != -1) {
                        if (pipe_choice == 0) game.settings.pipe_distance = 8;
                        else if (pipe_choice == 1) game.settings.pipe_distance = 12;
                        else if (pipe_choice == 2) game.settings.pipe_distance = 16;
                        else if (pipe_choice == 3) game.settings.pipe_distance = 20;
                        save_settings(&game, SETTINGS_FILENAME);
                    }
                } 
                else if (choice == 4) {
                    const char *gap_opts[] = {"2 (Hard)", "3 (Normal)", "4 (Easy)", "5 (Very Easy)"};
                    int gap_choice = ShowDialog(&game, "MAX GAPE SIZE", "Select maximum gap size", gap_opts, 4);
                    
                    if (gap_choice != -1) {
                        if (gap_choice == 0) game.settings.gapes_range = 2;
                        else if (gap_choice == 1) game.settings.gapes_range = 3;
                        else if (gap_choice == 2) game.settings.gapes_range = 4;
                        else if (gap_choice == 3) game.settings.gapes_range = 5;
                        save_settings(&game, SETTINGS_FILENAME);
                    }
                } 
                else if (choice == 5) {
                    const char *reset_opts[] = {"Yes, restart record", "No, keep it"};
                    int reset_choice = ShowDialog(&game, "RESTART RECORD", "Are you sure you want to reset?", reset_opts, 2);
                    
                    if (reset_choice == 0) {
                        game.settings.user.best_score = 0;
                        save_settings(&game, SETTINGS_FILENAME);
                    }
                } 
                else if (choice == 6 || choice == -1) {
                    curr_state = STATE_MENU;
                }
                
                break;
            }
            /* playing */
            case STATE_PLAYING: {
                printf("\e[1;1H\e[2J");
                draw_info(game);
                initialize_game(&game);

                time_t last_time = time(NULL), curr_time;
                int frame_count = 0;
                bool jump_pressed = false;

                while (is_player_alive(game.player) && curr_state == STATE_PLAYING) {
                    
                    /* frames counter */
                    curr_time = time(NULL);
                    if (curr_time != last_time) {
                        game.current_fps = frame_count;
                        frame_count = 0;
                        last_time = curr_time;
                    }
                    frame_count++;

                    /* player input */
                    #if defined(_WIN32) || defined(_WIN64)
                    if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(VK_SPACE) & 0x8000) {
                        if (!jump_pressed) {
                            game.player.velocity_y = FLAP_FORCE; 
                            jump_pressed = true;
                        }
                    } else {
                        jump_pressed = false;
                    }
                    
                    /* pause menu */
                    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                        curr_state = STATE_MENU;
                        continue;
                    }
                    #endif

                    /* physics logic */
                    game.player.velocity_y += GRAVITY;
                    game.player.exact_y += game.player.velocity_y;
                    set_player_position(&game, game.player.position.x, (int)game.player.exact_y);
                    update_pipes(&game);
                    
                    if (collision(&game)) {
                        damage_player(&game.player, 100.0f); 
                    }
                    printf("\e[6;1H");

                    /* draw */
                    draw_stats(game);
                    update_screen(&game);
                    draw(game);
                    
                    #if defined(_WIN32) || defined(_WIN64)
                        Sleep(1000 / game.settings.fpslimit); 
                    #else
                        usleep(1000000 / game.settings.fpslimit); 
                    #endif
                }

                /* if we exit the loop because the player died, we switch to STATE_GAMEOVER */
                if (!is_player_alive(game.player)) {
                    curr_state = STATE_GAMEOVER;
                }
                break;
            }
            /* game over screen :( */
            case STATE_GAMEOVER: {
                if (game.player.score > game.settings.user.best_score) {
                    game.settings.user.best_score = game.player.score;
                    save_settings(&game, SETTINGS_FILENAME);
                }

                char go_desc[64];
                sprintf(go_desc, "Score: %d   |   Best: %d", game.player.score, game.settings.user.best_score);

                int go_choice = ShowDialog(&game, COLOUR_RED "      GAME OVER!", go_desc, game_over_options, 2);
                
                if (go_choice == 0) curr_state = STATE_PLAYING;
                else if (go_choice == 1 || go_choice == -1) curr_state = STATE_MENU;
                break;
            }
            
            case STATE_EXIT:
                break;
        }
    }
    cleanup_memory(&game);
    return 0;
}