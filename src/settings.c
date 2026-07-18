#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"

void default_settings(settings_t * settings) {
    settings->user.nickname[0] = '\0';
    settings->user.best_score = 0;
    settings->resolution.y = RES_HEIGHT;
    settings->resolution.x = RES_WIDTH;
    settings->pipe_distance = 20;
    settings->gapes_range = 3; 
    settings->fpslimit = 60;
}

bool load_settings(game_t *game, const char *s_file) {
    FILE *f = fopen(s_file, "r");
    if (f == NULL) {
        default_settings(&game->settings);
        return false;
    }
    /* checks if file is empty */
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0) {
        fclose(f);
        default_settings(&game->settings);
        return false;
    }
    fseek(f, 0, SEEK_SET);

    char line[64], key[32], str_value[32];
    int value;

    default_settings(&game->settings);

    while (fgets(line, sizeof(line), f) != NULL) {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '#') {
            continue;
        }

        if (sscanf(line, "%63[^=]=%63s", key, str_value) == 2) {
            char *carriage_return = strchr(str_value, '\r');
            if (carriage_return != NULL) {
                *carriage_return = '\0';
            }
            
            value = atoi(str_value);

            if (strcmp(key, "game.width") == 0) {
                game->settings.resolution.x = value;
            } 
            else if (strcmp(key, "game.height") == 0) {
                game->settings.resolution.y = value;
            }
            else if (strcmp(key, "game.fpslimit") == 0) {
                game->settings.fpslimit = value;
            }
            else if (strcmp(key, "map.pipes_distance") == 0) {
                game->settings.pipe_distance = value;
            }
            else if (strcmp(key, "map.gapes_range") == 0) {
                game->settings.gapes_range = value;
            }
            else if (strcmp(key, "user.record") == 0) {
                game->settings.user.best_score = value;
            }
            else if (strcmp(key, "user.name") == 0) {
                strncpy(game->settings.user.nickname, str_value, MAX_PLAYER_NAME - 1);
                game->settings.user.nickname[MAX_PLAYER_NAME - 1] = '\0';
            }
        }
    }

    fclose(f);
    return true;
}

bool save_settings(game_t *game, const char *s_file) {
    FILE *f = fopen(s_file, "w");
    if (f == NULL) 
        return false;

    fprintf(f, "# Settings :)\n");
    fprintf(f, "user.name=%s\n", game->settings.user.nickname);
    fprintf(f, "user.record=%d\n", game->settings.user.best_score);
    fprintf(f, "game.width=%d\n", game->settings.resolution.x);
    fprintf(f, "game.height=%d\n", game->settings.resolution.y);
    fprintf(f, "game.fpslimit=%d\n", game->settings.fpslimit);
    fprintf(f, "map.pipes_distance=%d\n", game->settings.pipe_distance);
    fprintf(f, "map.gapes_range=%d\n", game->settings.gapes_range);

    fclose(f);
    return true;
}