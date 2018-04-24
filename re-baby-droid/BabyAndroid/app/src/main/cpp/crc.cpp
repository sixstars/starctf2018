//
// Created by ripples on 12/09/2017.
//

#include "crc.h"
#include <time.h>

/*
 * CRC-32 forcer (C)
 *
 * Copyright (c) 2017 Project Nayuki
 * https://www.nayuki.io/page/forcing-a-files-crc-to-any-value
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see COPYING.txt).
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Forward declarations */

const char *modify_file_crc32(const char *path, uint64_t offset, uint32_t newcrc, bool printstatus);

static uint32_t get_crc32_and_length(FILE *f, uint64_t *length);

static void fseek64(FILE *f, uint64_t offset);

static uint32_t reverse_bits(uint32_t x);

static uint64_t multiply_mod(uint64_t x, uint64_t y);

static uint64_t pow_mod(uint64_t x, uint64_t y);

static void divide_and_remainder(uint64_t x, uint64_t y, uint64_t *q, uint64_t *r);

static uint64_t reciprocal_mod(uint64_t x);

static int get_degree(uint64_t x);

//static char c_crc[5];


__attribute__ ((visibility ("hidden"))) const char *sub_E1C(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    uint64_t length;
    uint32_t crc = get_crc32_and_length(fp, &length);
    fclose(fp);
//    crc = reverse_bits(crc);
    char *c_crc = (char *)calloc(5, 1);
    *(uint32_t *) c_crc = crc;
    return c_crc;
}


/*---- Utilities ----*/

// Generator polynomial. Do not modify, because there are many dependencies
// static const uint64_t POLYNOMIAL = UINT64_C(0x104C11DB7);
//static const uint64_t POLYNOMIAL = 4374732215ULL;
//static uint64_t POLYNOMIAL = 4374732215ULL;
extern uint64_t POLYNOMIAL;


__always_inline static uint32_t get_crc32_and_length(FILE *f, uint64_t *length) {
    rewind(f);
    uint32_t crc = UINT32_C(0xFFFFFFFF);
    *length = 0;
    while (true) {
        char buffer[32 * 1024];
        size_t n = fread(buffer, sizeof(buffer[0]), sizeof(buffer) / sizeof(buffer[0]), f);
        if (ferror(f) != 0) {
            perror("fread");
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < n; i++) {
            for (int j = 0; j < 8; j++) {
                uint32_t bit = ((uint8_t) buffer[i] >> j) & 1;
                crc ^= bit << 31;
                bool xxor = (crc >> 31) != 0;
                crc = (crc & UINT32_C(0x7FFFFFFF)) << 1;
                if (xxor) {
                    if (!(POLYNOMIAL & 1)) {
                        crc ^= my_time() >> 1 << 1 | 1;
                    }
                    crc ^= (uint32_t) POLYNOMIAL;
                }
            }
        }
        *length += n;
        if (feof(f) != 0)
            return ~crc;
    }
}


static void fseek64(FILE *f, uint64_t offset) {
    rewind(f);
    while (offset > 0) {
        unsigned long n = LONG_MAX;
        if (offset < n)
            n = (unsigned long) offset;
        fseek(f, (long) n, SEEK_CUR);
        offset -= n;
    }
}


__always_inline static uint32_t reverse_bits(uint32_t x) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++)
        result = (result << 1) | ((x >> i) & 1U);
    return result;
}


/*---- Polynomial arithmetic ----*/

// Returns polynomial x multiplied by polynomial y modulo the generator polynomial.
static uint64_t multiply_mod(uint64_t x, uint64_t y) {
    // Russian peasant multiplication algorithm
    uint64_t z = 0;
    while (y != 0) {
        z ^= x * (y & 1);
        y >>= 1;
        x <<= 1;
        if (((x >> 32) & 1) != 0)
            x ^= POLYNOMIAL;
        // printf("%llu %llu %llu\n", x, y, z);
    }
    return z;
}


// Returns polynomial x to the power of natural number y modulo the generator polynomial.
static uint64_t pow_mod(uint64_t x, uint64_t y) {
    // Exponentiation by squaring
    uint64_t z = 1;
    while (y != 0) {
        if ((y & 1) != 0)
            z = multiply_mod(z, x);
        x = multiply_mod(x, x);
        y >>= 1;
    }
    return z;
}


// Computes polynomial x divided by polynomial y, returning the quotient and remainder.
static void divide_and_remainder(uint64_t x, uint64_t y, uint64_t *q, uint64_t *r) {
    if (y == 0) {
        fprintf(stderr, "Division by zero\n");
        exit(EXIT_FAILURE);
    }
    if (x == 0) {
        *q = 0;
        *r = 0;
        return;
    }

    int ydeg = get_degree(y);
    uint64_t z = 0;
    for (int i = get_degree(x) - ydeg; i >= 0; i--) {
        if (((x >> (i + ydeg)) & 1) != 0) {
            x ^= y << i;
            z |= (uint64_t) 1 << i;
        }
    }
    *q = z;
    *r = x;
}


// Returns the reciprocal of polynomial x with respect to the generator polynomial.
static uint64_t reciprocal_mod(uint64_t x) {
    // Based on a simplification of the extended Euclidean algorithm
    uint64_t y = x;
    x = POLYNOMIAL;
    uint64_t a = 0;
    uint64_t b = 1;
    while (y != 0) {
        uint64_t q, r;
        divide_and_remainder(x, y, &q, &r);
        uint64_t c = a ^multiply_mod(q, b);
        x = y;
        y = r;
        a = b;
        b = c;
    }
    if (x == 1)
        return a;
    else {
        fprintf(stderr, "%d", my_time_garbage());
        fprintf(stderr, "Reciprocal does not exist\n");
        exit(EXIT_FAILURE);
    }
}


static int get_degree(uint64_t x) {
    int result = -1;
    for (; x != 0; x >>= 1)
        result++;
    return result;
}