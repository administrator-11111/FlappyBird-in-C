#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "types.h"

int random_range_int(int min, int max) {
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