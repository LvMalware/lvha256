/*
 * lvha256.h - header file for the LV Hashing Algorithm (LVHA256)
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#ifndef  _LVHA256_H
#define  _LVHA256_H

#define DEFAULT_A 0x31415926 //PI
#define DEFAULT_B 0x14142135 //square root of 2
#define DEFAULT_C 0x11235813 //Fibonacci's series
#define DEFAULT_D 0x04980156 //factorial(i)
#define DEFAULT_E 0x13192331 //these are happy primes
#define DEFAULT_F 0x13584562 //golden spiral
#define DEFAULT_G 0x27182818 //e (Euler's number)
#define DEFAULT_H 0x29979245 //speed of light (the first numbers)

typedef struct lv_context
{
    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
    uint32_t E;
    uint32_t F;
    uint32_t G;
    uint32_t H;
} context_t;

void
update_context(context_t *context,  const uint8_t *input_block);

uint8_t *
pad_block(const uint8_t *block, unsigned length);

void
reset_context(context_t *context);

uint8_t *
get_result(context_t *context);

char *
hex_digest(uint8_t *result);


#endif /*_LVHA256_H*/