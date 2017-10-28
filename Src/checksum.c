/*
 * checksum.c
 *
 *  Created on: 2017. okt. 28.
 *      Author: Komposzt
 */


#include <stdio.h>
#include <string.h>

int NMEAchecksum(const char *s)
{
    int c = 0;

    while(*s)
        c ^= *s++;

    return c;
}
