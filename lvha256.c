/*
 * lvha256.c - source for the LV Hashing Algorithm (LVHA256)
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
#include <stdint.h>
#include <stdlib.h>
#include "lvha256.h"

/* Left rotation */
#define L(x, y) ((x << y) & 0xFFFFFFFF) | (x >> (32 - y))
/* Right rotation */
#define R(x, y) (x >> y) | (x << (8 * sizeof(x) - y))
/* Temper functions: W, X, Y, Z */
#define W(a, b, c, d, e, f, g, h) ((a + b + c + d) ^ (e + ~f)) ^ (g & h)
#define X(a, b, c, d, e, f, g, h) ((a ^ b) + (c ^ d) + e) ^ (f + g + h)
#define Y(a, b, c, d, e, f, g, h) ((a + b) ^ (c + d) ^ e) + (f ^ g ^ h)
#define Z(a, b, c, d, e, f, g, h) (g + (c & d) + (h ^ a) ^ ~e) + ~b ^ f

void
reset_context(context_t *context)
{
    /*
     * Input: context_t * context - pointer for a context_t structure
     * This function will reset the context constants to their default values
     * This must be done everytime before hashing something
     */
    context->A = DEFAULT_A;
    context->B = DEFAULT_B;
    context->C = DEFAULT_C;
    context->D = DEFAULT_D;
    context->E = DEFAULT_E;
    context->F = DEFAULT_F;
    context->G = DEFAULT_G;
    context->H = DEFAULT_H;
}

uint8_t *
pad_block(const uint8_t *block, unsigned length)
{
    /*
     * Input: uint8_t * block - the block to be padded to a size of 32 bytes
     *        unsigned length - the count of bytes on the block
     * Output: A uint8_t * pointer to a block with 32 bytes
     * This function ensures that a block is 32 bytes long.
     * NOTE: if the block is bigger than the default size of 32 bytes, you must
     * divide it yourself into smaller blocks and call this function to each one
     * of them.
     */
     
    /* allocates an array of uint8_t with 32 positions */
    uint8_t *padded = (uint8_t *) calloc(32, sizeof(uint8_t));
    /* copy the input block */
    memcpy(padded, block, length * sizeof(uint8_t));
    /* checks if padding is necessary */
    if (length < 32)
    {
        /*  as the function calloc already filled all the position with 0, and
            the function memcpy only filled the first LENGTH positions, the
            remaining are already with 0's. So, we just need to add a 1 on the
            position LENGTH
         */
        padded[length] = 0x80;
    }
    return padded;
}

void
update_context(context_t *context, const uint8_t *input_block)
{
    /*
     * Input: context_t *context   - a pointer for a context_t structure
     *        uint8_t *input_block - a block to be processed into the context
     * This will update the internal state of the hashing algorithm (context)
     * doing some operations using the temper functions.
     */
    int i;
    /* a small kludge to turn the 32 input bytes of the block into numbers of
       32 bits
     */
    uint32_t *u = (uint32_t *) input_block;
    /* store the current state to performe the operations */
    uint32_t A = context->A;
    uint32_t B = context->B;
    uint32_t C = context->C;
    uint32_t D = context->D;
    uint32_t E = context->E;
    uint32_t F = context->F;
    uint32_t G = context->G;
    uint32_t H = context->H;

    //ROUND 1
    for (i = 0; i < 8; i++)
    {
        /* I won't explain exactly how theses operations are working, because
           it's a little tricky process. But basically, they're performing many
           bit-wise operations with these 32 bit integers and compressing the
           results.
         */
        A = W(u[i], A, B, C, D, E, F, G);
        B = X(u[i], H, A, B, C, D, E, F);
        C = Y(u[i], G, H, A, B, C, D, E);
        D = Z(u[i], F, G, H, A, B, C, D);
        E = W(u[i], E, F, G, H, A, B, C);
        F = X(u[i], D, E, F, G, H, A, B);
        G = Y(u[i], C, D, E, F, G, H, A);
        H = Z(u[i], B, C, D, E, F, G, H);
    }
    
    //ROUND 2

    for (i = 0; i < 8; i++)
    {
        /* again, more tricky processes being executed. But it's the same
           idea, except that this time we are also using bit-shifting rotation
           that is conditionally to right or to left, depending on a certain
           integer K that is generated from the current state.
         */
        uint32_t K = A % (i * 1 + 2) + B % (i * 2 + 3) + C % (i * 3 + 5) +
                     D % (i * 4 + 2) + E % (i * 5 + 3) + F % (i * 6 + 5) +
                     G % (i * 7 + 2) + H % (i * 8 + 3);

        int o = K % 2;
        int j = (3 * i + 1) % 8;

        A = W(X(K, u[i], A, B, C, D, E, F), H, K, u[j], G, F, E, D);
        A = o ? R(A, 2) : L(A, 19);
        B = Y(Z(K, u[j], B, C, D, E, F, A), A, K, u[i], H, G, F, E);
        B = o ? R(B, 3) : L(B, 17);
        C = Z(Y(K, u[i], C, D, E, F, A, B), B, K, u[j], A, H, G, F);
        C = o ? R(C, 5) : L(C, 13);
        D = X(W(K, u[j], D, E, F, A, B, C), C, K, u[i], B, A, H, G);
        D = o ? R(D, 7) : L(D, 11);
        E = X(Z(K, u[i], E, F, A, B, C, D), D, K, u[j], C, B, A, H);
        E = o ? R(E, 11) : L(A, 7);
        F = Z(X(K, u[j], F, A, B, C, D, E), E, K, u[i], D, C, B, A);
        F = o ? R(F, 13) : L(F, 5);
        G = Y(W(K, u[i], A, B, C, D, E, F), F, K, u[j], E, D, C, B);
        G = o ? R(G, 17) : L(G, 3);
        H = W(Y(K, u[j], B, C, D, E, F, A), G, K, u[i], F, E, D, C);
        H = o ? R(H, 19) : L(H, 2);
    }
    /*
        At the end, we add everyting into the context, performing more bit-wise
        operations.
     */
    context->A = R(context->A + A, 7)  & 0xFFFFFFFF;
    context->B = L(context->B + B, 5)  & 0xFFFFFFFF;
    context->C = R(context->C + C, 3)  & 0xFFFFFFFF;
    context->D = L(context->D + D, 2)  & 0xFFFFFFFF;
    context->E = R(context->E + E, 19) & 0xFFFFFFFF;
    context->F = L(context->F + F, 17) & 0xFFFFFFFF;
    context->G = R(context->G + G, 13) & 0xFFFFFFFF;
    context->H = L(context->H + H, 11) & 0xFFFFFFFF;
}

uint8_t *
get_result(context_t * context)
{
    /*
     * Input: context_t *context - a state of the hashing algorithm.
     * Output: an array of 32 uint8_t numbers (32 bytes)
     * This takes a context and turns it into an array of 32 bytes, that are the
     * hashed form of the input data. (32 bytes = 256 bits)
     */
    uint8_t *result = (uint8_t *) calloc(32, sizeof(uint8_t));
    int x, i = 0;
    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->A >> x) & 0xFF;
    }

    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->B >> x) & 0xFF;
    }

    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->C >> x) & 0xFF;
    }

    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->D >> x) & 0xFF;
    }

    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->E >> x) & 0xFF;
    }

    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->F >> x) & 0xFF;
    }

    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->G >> x) & 0xFF;
    }

    for (x = 24; x >= 0; x -= 8)
    {
        result[i++] = (context->H >> x) & 0xFF;
    }

    return result;
}

char *
hex_digest(uint8_t *result)
{
    /*
     * Input: uint8_t *result - an array of 32 bytes (returned by get_result )
     * Output: a hexadecimal representation of the input
     * This takes the output of the get_result() function and returns its
     * hexadecimal representation for pretty printing.
     */
    char *digest = (char *) calloc(65, sizeof(char));
    int i;
    for (i = 0; i < 64; i += 2)
    {
        sprintf(&digest[i], "%02x", result[i/2]);
    }
    return digest;
}
