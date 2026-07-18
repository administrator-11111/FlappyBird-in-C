#ifndef SETTINGS_H
    #define SETTINGS_H
    
    #include <stdbool.h>
    #include "types.h"

    bool load_settings(game_t *, const char *);
    bool save_settings(game_t *, const char *);
    int get_buffer_size(game_t);
#endif