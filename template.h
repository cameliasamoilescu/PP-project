#ifndef TEMPLATE_H_INCLUDED
#define TEMPLATE_H_INCLUDED

#include <math.h>
#include "encryption.h"

typedef struct COORDINATES
{
    int x, y;

} COORDINATES;


typedef struct DETECTIONS
{
    COORDINATES top_left;
    unsigned int patternHeight;
    unsigned int patternWidth;
    double correlation;
    PIXEL color;
} DETECTIONS;


double getCorrelation(PIXEL *, PIXEL *, int, int, int, int, unsigned int);
DETECTIONS *Template_mtch(PIXEL*, char*, double, int*, PIXEL, unsigned int, unsigned int);
void template(char* image,  char *newImage, double ps);


void outline(PIXEL**, unsigned int, DETECTIONS, PIXEL);
char** getNameOfTemplates(char*, int*);
PIXEL* set_colors();
DETECTIONS* get_detections(char*, char**, int, int*, double);
int cmp(const void*, const void*);
void sortedDetections(DETECTIONS**, int);
double overlap(DETECTIONS, DETECTIONS);
DETECTIONS* delete_non_maximum(char*, char**, int, double, int*);

#endif
