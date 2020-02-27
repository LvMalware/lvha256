/*
 * test.c - source for a demonstartion of the LV Hashing Algorithm (LVHA256)
 *
 * Copyright (C) 2020 Lucas V. Araujo <lucas.vieira.ar@disroot.org> .
 *
 * This is free software: you are free to change and redistribute it under the
 * terms of GNU GPL license version 3 or, at your choice, any later version.
 * You should have received a copy of the license along with this software. If
 * not, you can read the full text at https://gnu.org/licenses/gpl.html.
 * This software is offered AS IT IS, in hope that it will be usefull, but there
 * is NO WARRANTY, to the extent permitted by law.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lvha256.h"

int
main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s [-s <text>] <input_file>\n", argv[0]);
        return 0;
    }
    
    context_t mycontext;
    reset_context(&mycontext);

    if (argv[1][0] == '-' && argv[1][1] == 's' && strlen(argv[1]) == 2)
    {
        uint8_t *input  = (uint8_t *) argv[2];
        unsigned i, length = strlen(argv[2]);
        for (i = 0; i < length; i += 32)
        {
            if ((i + 32) > length)
            {
                uint8_t *block = pad_block(&input[i], length - i);
                update_context(&mycontext, block);
                free(block);
            }
            else
            {
                update_context(&mycontext, &input[i]);
            }
        }
    }
    else
    {
        FILE *input_file = fopen(argv[1], "rb");
        if (!input_file)
        {
            printf("Error opening file %s\n", argv[1]);
            return 1;
        }
        while(!feof(input_file))
        {
            uint8_t block[32];
            unsigned count = fread(block, sizeof(uint8_t), 32, input_file);
            uint8_t *padded = pad_block(block, count);
            update_context(&mycontext, padded);
            free(padded);
        }
        fclose(input_file);
    }
    uint8_t *result = get_result(&mycontext);
    char *digest = hex_digest(result);
    free(result);
    printf("%s\n", digest);
    free(digest);
    return 0;
}