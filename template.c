#include "template.h"



double getCorrelation(PIXEL *image, PIXEL *pattern, int top_left_x, int top_left_y, int patternWidth, int patternHeight, unsigned int width)
{

    int pixelsNo = patternHeight * patternWidth;
    double img_average, pattern_average, img_standard_deviation, pattern_standard_deviation;
    double img_sum = 0, pattern_sum = 0,img_sdSum = 0, pattern_sdSum = 0;

    int i, j;
    unsigned int img, ptn;

    for(i = 0; i < patternHeight; ++ i)
        for(j = 0; j < patternWidth; ++ j)
        {
            img_sum += (image[(i + top_left_x) * width + (j + top_left_y)].R);
            pattern_sum += pattern[i * patternWidth + j].R;
        }


    img_average = img_sum / pixelsNo;
    pattern_average = pattern_sum / pixelsNo;

    for(i = 0; i < patternHeight; ++ i)
        for(j = 0; j < patternWidth; ++ j)
        {

            img = image[(i + top_left_x) * width + (j + top_left_y)].R;
            ptn = pattern[i * patternWidth + j].R;

            img_sdSum += (img - img_average)*(img - img_average);
            pattern_sdSum += (ptn - pattern_average)*(ptn - pattern_average);
        }

    // sd = standard deviation
    img_standard_deviation = sqrt(1.0 / (pixelsNo - 1) * img_sdSum);
    pattern_standard_deviation = sqrt(1.0 / (pixelsNo - 1) * pattern_sdSum);


    double correl = 0;

    for(i = 0; i < patternHeight; ++ i)
        for(j = 0; j < patternWidth; ++ j)
        {

            img = image[(i + top_left_x) * width + (j + top_left_y)].R;
            ptn = pattern[i * patternWidth + j].R;

            correl += 1.0 / (img_standard_deviation * pattern_standard_deviation) * (img - img_average) * (ptn - pattern_average);
        }

    correl = 1.0 / pixelsNo * correl;
    return correl;
}


DETECTIONS *Template_mtch(PIXEL *linearIMG, char *ptn, double ps, int *detectionsNo, PIXEL color, unsigned int width, unsigned int height)
{
    char *pattern = (char*)"GrayscalePattern.bmp";

    grayscale_image(ptn, pattern);

    PIXEL* linearPattern = linear_image(pattern);


    unsigned int patternWidth, patternHeight;

    getWidthHeight(pattern, &patternWidth, &patternHeight);

    int i, j;


    double pattern_size = patternWidth * patternHeight;

    int noDet = 0;
    double corr = 0;

    DETECTIONS* detections = (DETECTIONS*) calloc(width * height, sizeof(DETECTIONS));

    if(detections == NULL)
    {
        perror("Malloc error:\n");
        printf("Insufficient memory for detections\n");
        return 0;
    }
    for(i = 0; i < height - patternHeight; ++ i)
    {
        for(j = 0; j < width - patternWidth; ++ j)
        {

            corr = getCorrelation(linearIMG, linearPattern, i, j, patternWidth, patternHeight, width);

            if(corr >= ps)
            {
                detections[noDet].top_left.x = i;
                detections[noDet].top_left.y = j;
                detections[noDet].patternHeight = patternHeight;
                detections[noDet].patternWidth = patternWidth;
                detections[noDet].correlation = corr;
                detections[noDet].color = color;
                noDet ++;
            }
        }
    }

    (*detectionsNo) = noDet;
    free(linearPattern);
    return detections;
}


void template(char* image,  char *newImage, double ps)
{


    grayscale_image(image, newImage);
    unsigned int width, height;
    getWidthHeight(newImage, &width, &height);

    PIXEL* linearIMG = linear_image(image);


    int noDetections = 0;

    char *fileTemplates = (char*)"information.txt";
    int noTemplates;
    char **nameOfTemplates ;
    nameOfTemplates = getNameOfTemplates(fileTemplates, &noTemplates);

    DETECTIONS *Detections = delete_non_maximum(newImage, nameOfTemplates, noTemplates, ps, &noDetections);

    for(int index = 0; index < noDetections; ++index)
    {
        outline(&linearIMG, width, Detections[index], Detections[index].color);
    }

    createImage(image, (char*) "newImage.bmp",linearIMG);
    free(nameOfTemplates);
    free(Detections);
    free(linearIMG);
}


void outline(PIXEL **linearIMG, unsigned int width, DETECTIONS f, PIXEL color)
{
    COORDINATES top_left, button_right;
    top_left.x = f.top_left.x;
    top_left.y = f.top_left.y;

    button_right.x = top_left.x + f.patternHeight - 1;
    button_right.y = top_left.y + f.patternWidth - 1;
    int i, j;

    for(j = top_left.y; j <= button_right.y; ++j)
    {
        (*linearIMG)[top_left.x * width + j] = color;
        (*linearIMG)[button_right.x * width + j] = color;
    }

    for(i = top_left.x; i < button_right.x; ++i)
    {
        (*linearIMG)[i * width + top_left.y] = color;
        (*linearIMG)[i * width + button_right.y] = color;
    }
}



char** getNameOfTemplates(char *file_name, int *noTemplates)
{
    FILE *f = fopen(file_name, "r");

    if(f == NULL)
    {
        printf("(getNameofTemplates) Couldn't find the file\n");
        return 0;
    }
    int i, n;

    fscanf(f,"%d\n", &n);


    char **nameOfTemplates = (char**)malloc(sizeof(char*) * (2 * n));

    for(i = 0; i < n; i ++)
    {
        nameOfTemplates[i] = (char*) malloc(20);
        fscanf(f, "%s\n", nameOfTemplates[i]);
    }

    (*noTemplates) = n;
    return nameOfTemplates;

}



PIXEL* set_colors()
{
    PIXEL* colors = (PIXEL*) malloc(sizeof(PIXEL) * 12);

    colors[0].R = 255;
    colors[0].G = 0;
    colors[0].B = 0;

    colors[1].R = 255;
    colors[1].G = 255;
    colors[1].B = 0;

    colors[2].R = 0;
    colors[2].G = 255;
    colors[2].B = 0;

    colors[3].R = 0;
    colors[3].G = 255;
    colors[3].B = 255;

    colors[4].R = 255;
    colors[4].G = 0;
    colors[4].B = 255;

    colors[5].R = 0;
    colors[5].G = 0;
    colors[5].B = 255;

    colors[6].R = 192;
    colors[6].G = 192;
    colors[6].B = 192;

    colors[7].R = 255;
    colors[7].G = 140;
    colors[7].B = 0;

    colors[8].R = 128;
    colors[8].G = 0;
    colors[8].B = 128;

    colors[9].R = 128;
    colors[9].G = 0;
    colors[9].B = 0;

    return colors;
}



DETECTIONS* get_detections(char *image, char** nameOfTemplates, int noTemplates, int *detection_no, double ps)
{

    int i, k;
    int noDETECTIONS = 0;


    PIXEL* linearIMG = linear_image(image);

    unsigned int width, height;

    getWidthHeight(image, &width, &height);

    DETECTIONS *D = (DETECTIONS*) malloc(sizeof(DETECTIONS) * width * height * 10);

    PIXEL *color = set_colors();

    for(i = 0; i < noTemplates; i ++)
    {
        int detNo;

        DETECTIONS* det = Template_mtch(linearIMG, nameOfTemplates[i], ps, &detNo, color[i], width, height);

        for(k = 0; k < detNo; k ++)
                    D[noDETECTIONS ++] = det[k];
        free(det);
    }
    (*detection_no) = noDETECTIONS;
    free(linearIMG);
    free(color);
    return D;

}



int cmp(const void *a, const void *b)
{
    DETECTIONS x = *(DETECTIONS*) a;
    DETECTIONS y = *(DETECTIONS*) b;

    if(x.correlation > y.correlation)
        return -1;
    else if(x.correlation < y.correlation)
                return 1;
    return 0;
}



void sortedDetections(DETECTIONS **D, int noDETECTIONS)
{

    qsort(*D, noDETECTIONS, sizeof(DETECTIONS), cmp);

}



double overlap(DETECTIONS a, DETECTIONS b)
{

    int heightIntersection = a.patternHeight - (a.top_left.x > b.top_left.x ? a.top_left.x - b.top_left.x : b.top_left.x - a.top_left.x);
    int widthIntersection = a.patternWidth - (a.top_left.y > b.top_left.y ? a.top_left.y - b.top_left.y : b.top_left.y - a.top_left.y);

    if(widthIntersection < 0 || heightIntersection < 0)
                               return 0;

            double intersectionArea = widthIntersection * heightIntersection;
            double patternArea = a.patternWidth * a.patternHeight;

            double overl = (float)intersectionArea / (patternArea + patternArea - intersectionArea);

            if(overl > 0.2)
                return 1;

    return 0;
}



DETECTIONS* delete_non_maximum(char *image, char **nameOfTemplates, int noTemplates, double ps, int *newNoDETECTIONS)
{
    int noDETECTIONS, i,j;
    DETECTIONS *D = get_detections(image, nameOfTemplates, noTemplates, &noDETECTIONS, ps);

    sortedDetections(&D, noDETECTIONS);

    *newNoDETECTIONS = 0;
    for(i = 0; i < noDETECTIONS - 1; ++ i)
                for(j = i + 1; j < noDETECTIONS ; ++ j)
                        if(overlap(D[i], D[j]) == 1)
                            D[i].correlation = 3.0;

            DETECTIONS *D_nou = (DETECTIONS*)malloc(sizeof(DETECTIONS) * 10000000);

            for(i = 0; i < noDETECTIONS; i ++)
                    if(D[i].correlation != 3.0)
                        D_nou[(*newNoDETECTIONS)++] = D[i];
            free(D);
            return D_nou;

}

