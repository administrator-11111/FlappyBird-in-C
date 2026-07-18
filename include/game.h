#ifndef GAME_H
    #define GAME_H

    #include <stdbool.h>
    #include "types.h"

    /* MAIN DECLARATIONS */
    void initialize_game(game_t *);
    bool collision(game_t *);

    /* PIPE DECLARATIONS */
    void create_pipe(game_t *, int, int, int, int);
    void update_pipes(game_t *);

    /* PLAYER DECLARATIONS */
    bool is_player_alive(player_t);
    float set_player_health(player_t *, float);
    int set_player_score(player_t *, int);
    bool set_player_position(game_t *, int, int);
    bool move_player(game_t *, int, int);
    bool damage_player(player_t *, float);
    bool heal_player(player_t *, float);
    bool increase_player_score(player_t *, int);
    bool decrease_player_score(player_t *, int);

    /* DYNAMIC SIZES DECLARATIONS */
    int get_game_height(game_t *game);
    int get_game_width(game_t *game);
    int get_min_gap(game_t *game);
    int get_max_gap(game_t *game);
    int get_active_pipes(game_t *game);
    
#endif