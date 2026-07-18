#include <stdbool.h>
#include "game.h"
#include "utils.h"
#include "render.h"

/* 
    Player utilities 
*/
bool is_player_alive(player_t player) { return (player.health > 0.0f); }

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

bool set_player_position(game_t *game, int x, int y) {
    /* horizontal pos */
    if (x < 1) x = 1; 
    else if (x > game->settings.resolution.x - get_game_width(game)) x = game->settings.resolution.x - get_game_width(game);
    
    /* vertical pos */
    if (y < 1) {
        y = 1;
        game->player.velocity_y = 0.0f; 
        game->player.exact_y = (float)y;
    } else if (y > game->settings.resolution.y - get_game_height(game)) {
        y = game->settings.resolution.y - get_game_height(game);
        game->player.velocity_y = 0.0f;
        game->player.exact_y = (float)y;
    }
    game->player.position.x = x;
    game->player.position.y = y;
    return true;
}

bool move_player(game_t *game, int dx, int dy) {
    return set_player_position(game, game->player.position.x + dx, game->player.position.y + dy);
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
    Collisions
*/
static bool _check_edge_collision(game_t *game) {
    return (game->player.position.y <= 0 || game->player.position.y >= game->settings.resolution.y || game->player.position.x <= 0 || game->player.position.x >= game->settings.resolution.x);
}

static bool _check_pipe_collision(game_t *game) {
    for (int i= 0; i < get_active_pipes(game); i++) {
        if ((game->player.position.x + get_game_width(game)) > (game->pipe[i].position.x - 1) && (game->player.position.x) <= (game->pipe[i].position.x + 1)) {
            if ((game->player.position.y) < (game->pipe[i].position.y - game->pipe[i].gap_size) || (game->player.position.y + get_game_height(game)) > (game->pipe[i].position.y + game->pipe[i].gap_size)) {
                return true;
            }
        }
    }
    return false;
}

bool collision(game_t *game) {
    return (_check_edge_collision(game) || _check_pipe_collision(game));
}

/* 
    Pipeline functions 
*/
void create_pipe(game_t *game, int index, int x, int y, int gap_size) {
    if (index < 0 || index >= get_active_pipes(game)) return;
    game->pipe[index].position.x = x;
    game->pipe[index].position.y = y;
    game->pipe[index].gap_size = gap_size;
}

void update_pipes(game_t *game) {
    for (int i = 0; i < get_active_pipes(game); i++) {
        game->pipe[i].position.x--;

        if (game->pipe[i].position.x == game->player.position.x) {
            increase_player_score(&game->player, 1);
        }

        if (game->pipe[i].position.x < 1) {
            int max_x = 0;
            for (int j = 0; j < get_active_pipes(game); j++) {
                if (game->pipe[j].position.x > max_x) max_x = game->pipe[j].position.x;
            }

            int new_x = max_x + game->settings.pipe_distance;
            int range = game->settings.gapes_range;
            int safe_margin = range + 2;

            int new_y = random_range_int(safe_margin, game->settings.resolution.y - safe_margin);
        
            create_pipe(game, i, new_x, new_y, range);
        }
    }
}

void initialize_game(game_t *game) {
    game->player.exact_y = (float)(game->settings.resolution.y / 2);
    game->player.velocity_y = 0.0f;

    set_player_position(game, game->settings.resolution.x / 4, (int)game->player.exact_y);
    set_player_score(&game->player, 0);
    set_player_health(&game->player, 100.0f);

    initialize_screen(game);
    
    for (int i = 0; i < get_active_pipes(game); i++) {
        int gap = game->settings.gapes_range;
        int safe_margin = gap + 2;
        
        int start_x = game->settings.resolution.x + (i * game->settings.pipe_distance);
        int start_y = random_range_int(safe_margin, game->settings.resolution.y - safe_margin);
        
        create_pipe(game, i, start_x, start_y, gap);
    }
}

/* 
    DYNAMIC SIZES
*/
int get_game_height(game_t *game) {
    int h = game->settings.resolution.y / 12;
    return h < 1 ? 1 : h;
}
int get_game_width(game_t *game) {
    return get_game_height(game);
}
int get_min_gap(game_t *game) {
    return get_game_height(game) + 1;
}
int get_max_gap(game_t *game) {
    return get_min_gap(game) + (game->settings.resolution.y / 8);
}
int get_active_pipes(game_t *game) {
    int active = (game->settings.resolution.x / game->settings.pipe_distance) + 2;
    return active > MAX_PIPES ? MAX_PIPES : active;
}