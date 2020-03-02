#ifndef ENCRYPTION_H_INCLUDED
#define ENCRYPTION_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>


typedef struct PIXEL
{
    unsigned char B,G,R;
} PIXEL;


unsigned int XORSHIFT32 (unsigned int*);
void grayscale_image(char*, char*);
void getWidthHeight(char*, unsigned int*, unsigned int*);
void createImage(char*, char*, PIXEL*);
PIXEL* linear_image(char*);
unsigned int* R_Generation(int, unsigned int);
unsigned int* random_permutation(unsigned int, unsigned int);
PIXEL* pixels_permutation(PIXEL*, unsigned int, unsigned int, unsigned int);
PIXEL xor_pixel_pixel(PIXEL, PIXEL);
PIXEL xor_pixel_int(PIXEL, unsigned int);
void encryption(char*, char*, char*);
void decryption(char*, char*, char*, char*);
void Chi2Test(char*);


#endif