#ifndef RENDER_H
    #define RENDER_H

    #include <stdbool.h>
    #include "types.h"
    
    /* MEMORY SCREEN*/
    bool allocate_screen_memory(game_t *);
    void cleanup_memory(game_t *);
    int get_buffer_size(game_t);
    
    /* DRAW */
    void initialize_screen(game_t *);
    void update_screen(game_t *);
    void draw(game_t);
    void draw_stats(game_t);
    void draw_info(game_t);
    
    /* UI */
    int ShowDialog(game_t *, const char *, const char *, const char **, int);
#endif