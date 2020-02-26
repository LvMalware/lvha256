/*
 * test.c - testing the securiy of the LV Hashing Algorithm (LVHA256)
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
#include "../lvha256.h"

int
main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <text>\n", argv[0]);
        return 0;
    }
    uint8_t *input_data = (uint8_t *) argv[1];
    unsigned length = strlen(argv[1]);
    context_t mycontext;
    reset_context(&mycontext);
    int i;
    for (i = 0; i < length; i += 32)
    {
        unsigned count = ((i + 32) > length) ? length - i : 32;
        uint8_t *padded = pad_block(&input_data[i], count);
        update_context(&mycontext, padded);
        free(padded);
    }
    
    uint8_t *result = get_result(&mycontext);
    char *digest = hex_digest(result);
    free(result);
    printf("%s\n", digest);
    free(digest);
    return 0;
}