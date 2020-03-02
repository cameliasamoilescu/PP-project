#include "encryption.h"


unsigned int XORSHIFT32 (unsigned int *seed)
{
    unsigned int x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *seed = x;

    return x;
}



void grayscale_image(char* source_file_name, char* dest_file_name)
{
    FILE *fin, *fout;
    unsigned int img_dim, img_width, img_height;
    unsigned char pRGB[3], aux;

    //printf("source_file_name = %s \n",source_file_name);

    fin = fopen(source_file_name, "rb");
    if(fin == NULL)
    {
        printf("(grayscale_image) couldn't find the source_file \n");
        return;
    }

    fout = fopen(dest_file_name, "wb+");

    fseek(fin, 2, SEEK_SET);
    fread(&img_dim, sizeof(unsigned int), 1, fin);
    //printf("Image size in bytes: %u\n", img_dim);

    fseek(fin, 18, SEEK_SET);
    fread(&img_width, sizeof(unsigned int), 1, fin);
    fread(&img_height, sizeof(unsigned int), 1, fin);
    //printf("Image size in pixels (width x height): %u x %u\n",img_width, img_height);


    fseek(fin,0,SEEK_SET);
    unsigned char c;
    while(fread(&c,1,1,fin)==1)
    {
        fwrite(&c,1,1,fout);
        fflush(fout);
    }
    fclose(fin);

    // padding for a line
    int padding;
    if(img_width % 4 != 0)
        padding = 4 - (3 * img_width) % 4;
    else
        padding = 0;

    //printf("padding = %d \n",padding);

    fseek(fout, 54, SEEK_SET);
    unsigned int i,j;
    for(i = 0; i < img_height; i++)
    {
        for(j = 0; j < img_width; j++)
        {
            //colors of the pixel
            fread(pRGB, 3, 1, fout);

            //convert to gray pixel
            aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
            pRGB[0] = pRGB[1] = pRGB[2] = aux;
            fseek(fout, -3, SEEK_CUR);
            fwrite(pRGB, 3, 1, fout);
            fflush(fout);
        }
        fseek(fout,padding,SEEK_CUR);
    }
    fclose(fout);
}



void getWidthHeight(char *file_name, unsigned int *img_width, unsigned int *img_height)
{
    FILE *f = fopen(file_name, "rb");

    if(f == NULL)
    {
        printf("(getWidthHeight) couldn't find the image\n");
        return ;
    }

    fseek(f, 18, SEEK_SET);
    fread(img_width, sizeof(unsigned int), 1, f);
    fread(img_height, sizeof(unsigned int), 1, f);

    fclose(f);
}



void createImage(char *image, char *formed_image, PIXEL* linearImg)
{

    FILE *f = fopen(image, "rb");

    unsigned int img_height, img_width;

    getWidthHeight(image, &img_width, &img_height);



    FILE *g = fopen(formed_image, "wb+");
    if(g == NULL)
    {
        printf("(create image) error when opening the decrypted image file\n");
        return;
    }
    int padding;
    if(img_width % 4 != 0)
        padding = 4 - (3 * img_width) % 4;
    else
        padding = 0;

    PIXEL *padding_ad;
    padding_ad =(PIXEL*) malloc(sizeof(PIXEL) * padding);

    unsigned char c;
    int i, j,k;

    fseek(f,0L,SEEK_SET);


    for(i = 0; i < 54; i ++)
    {
        fread(&c,1,1,f);
        fwrite(&c,1,1,g);
        fflush(g);
    }


    int t;
    for(i = 0; i < img_height; i ++)
    {
        for(j = 0; j < img_width; j ++)
        {
            fwrite(&linearImg[i * img_width + j], 3, 1, g);
            fflush(g);
        }
        for(k = 0; k < padding; ++k)
            fwrite(&t, 1, 1, g);
    }

    fclose(f);
    fclose(g);
}



PIXEL* linear_image(char *file_name)
{
    FILE *f = fopen(file_name, "rb");
    PIXEL *v;
    unsigned int i, j, k;
    if(f == NULL)
    {
        printf("(linear_image) couldn't find the image\n");
        return 0;
    }


    unsigned int img_width, img_height;

    getWidthHeight(file_name, &img_width, &img_height);

    unsigned int size = img_height * img_width;
    v = (PIXEL*) malloc(size * sizeof(PIXEL));

    if (v == NULL)
    {
        printf("insufficient memory for linearization\n");
        return 0;
    }

    int padding;
    if(img_width %4  != 0)
        padding = 4 - (3 * img_width)%4;
    else
        padding = 0;

    PIXEL p;
    char t;
    fseek(f, 54, SEEK_SET);

    for(i = 0; i < img_height; i ++)
    {
        for(j = 0; j < img_width; j ++)
        {
            fread(&p, 3, 1, f);
            v[i * img_width + j] = p;
        }
        for(k = 0; k < padding; ++k)
            fread(&t, 1, 1, f);
    }

    fclose(f);
    return v;
}




unsigned int* R_Generation(int n, unsigned int seed)
{
    unsigned int* R = (unsigned int*) malloc(sizeof(unsigned int) * n);

    int i;
    R[0] = seed;
    for(i = 1; i < n; i ++)
        R[i] = XORSHIFT32(&seed);


    return R;
}



unsigned int* random_permutation(unsigned int n, unsigned int seed)
{
    unsigned int *P = (unsigned int*) malloc(sizeof(unsigned int) * n);
    unsigned int *R = R_Generation(n, seed);
    unsigned int j, i;
    for(i = 0; i < n; i ++)
        P[i] = i;


    unsigned int aux;

    for(i = n - 1; i >= 1; i --)
    {
        j = R[n-i] % (i + 1);
        aux = P[j];
        P[j] = P[i];
        P[i] = aux;

    }
    free(R);
    return P;
}



PIXEL* pixels_permutation(PIXEL *image, unsigned int img_height, unsigned int img_width, unsigned int seed)
{

    PIXEL *image_permuted_pixels = (PIXEL*) malloc(img_height * img_width * sizeof(PIXEL));

    unsigned int i;
    unsigned int *permutation = random_permutation(img_width * img_height, seed);

    for(i = 0; i < img_height * img_width; i ++)
        image_permuted_pixels[permutation[i]] = image[i];
    free(permutation);
    return image_permuted_pixels;

}



PIXEL xor_pixel_pixel(PIXEL a, PIXEL b)
{
    PIXEL c;

    c.B = a.B ^ b.B;
    c.G = a.G ^ b.G;
    c.R = a.R ^ b.R;

    return c;
}



PIXEL xor_pixel_int(PIXEL a, unsigned int x)
{
    PIXEL c;
    c.B = a.B ^ (x & 255);
    x = x >> 8;
    c.G = a.G ^ (x & 255);
    x = x >> 8;
    c.R = a.R ^ (x & 255);
    return c;
}

void encryption(char *image, char *encrypted_img_name, char *key_file_name)
{

    FILE *f = fopen(key_file_name, "r");

    if(f == NULL)
    {
        printf("(encryption) error when opening the secret_key file\n");
        return;
    }
    unsigned int seed;
    fscanf(f, "%u", &seed);

    unsigned int starting_value;
    fscanf(f, "%u", &starting_value);

    fclose(f);

    PIXEL *linearImg = linear_image(image);

    unsigned int img_height, img_width;

    getWidthHeight(image, &img_width, &img_height);

    unsigned int img_dim = img_height * img_width;

    PIXEL *permuted_pixels = pixels_permutation(linearImg, img_height, img_width, seed);

    PIXEL *encrypted_img = (PIXEL*) malloc(sizeof(PIXEL) * img_dim);

    unsigned int *sequence = R_Generation(2 * img_height * img_width - 1, seed);

    encrypted_img[0] = xor_pixel_int(xor_pixel_int(permuted_pixels[0], starting_value), sequence[img_dim]);

    unsigned int i, j;
    for(i = 1; i < img_dim; i ++)
        encrypted_img[i] = xor_pixel_int(xor_pixel_pixel(encrypted_img[i - 1], permuted_pixels[i]), sequence[img_dim + i]);

    createImage(image, encrypted_img_name, encrypted_img);
    free(sequence);
    free(encrypted_img);
}




void decryption(char *image, char *encrypted_img_name, char *decrypted_img_name, char *key_file_name)
{
    FILE *f = fopen(key_file_name, "r");

    if(f == NULL)
    {
        printf("(decryption) error when opening the secret_key file\n");
        return;
    }
    unsigned int seed;
    fscanf(f, "%u", &seed);

    unsigned int starting_value;
    fscanf(f, "%u", &starting_value);

    fclose(f);

    PIXEL *LinearImg = linear_image(encrypted_img_name);

    unsigned int img_height, img_width;

    getWidthHeight(image, &img_width, &img_height);

    unsigned int img_dim = img_height * img_width;

    unsigned int *sequence = R_Generation(2 * img_dim - 1, seed);

    unsigned int *permutation = random_permutation(img_dim, seed);

    int i, j;

    int *inverse_permutation = (int*) malloc(img_dim * sizeof(int));

    for(i = 0; i < img_height * img_width; i ++)
        inverse_permutation[permutation[i]] = i;

    PIXEL *intermediate_encryp_img = (PIXEL*) malloc(img_dim * sizeof(PIXEL));

    intermediate_encryp_img[0] = xor_pixel_int(xor_pixel_int(LinearImg[0], sequence[img_dim]), starting_value);
    for(i = 1; i < img_dim; i ++)
        intermediate_encryp_img[i] = xor_pixel_int(xor_pixel_pixel(LinearImg[i-1], LinearImg[i]), sequence[img_dim + i]);

    PIXEL *decrypted_img = (PIXEL*) malloc (img_dim * sizeof(PIXEL));

    for(i = 0; i < img_dim; i ++)
        decrypted_img[inverse_permutation[i]] = intermediate_encryp_img[i];

    createImage(image, decrypted_img_name, decrypted_img);
    free(LinearImg);
    free(intermediate_encryp_img);
    free(decrypted_img);
    free(sequence);
    free(permutation);
    free(inverse_permutation);
}



void Chi2Test(char *image)
{
    PIXEL *linearImg = linear_image(image);

    unsigned int img_height, img_width, img_dim;

    getWidthHeight(image,&img_width, &img_height);
    img_dim = img_height * img_width;

    int *R_array = (int *) calloc(257, sizeof(int));
    int *G_array = (int *) calloc(257, sizeof(int));
    int *B_array = (int *) calloc(257, sizeof(int));

    int i;
    double Chi_R = 0, Chi_G = 0, Chi_B = 0;

    for(i = 0; i < img_dim; i ++)
    {
        R_array[linearImg[i].R] ++;
        G_array[linearImg[i].G] ++;
        B_array[linearImg[i].B] ++;
    }

    double estimated_freq = img_dim / 256;

    for(i = 0; i <= 255; i ++)
    {
        double R = R_array[i] - estimated_freq;
        double G = G_array[i] - estimated_freq;
        double B = B_array[i] - estimated_freq;
        Chi_R += R * R/estimated_freq;
        Chi_G += G * G/estimated_freq;
        Chi_B += B * B/estimated_freq;
    }

    free(B_array);
    free(G_array);
    free(R_array);

    printf("Chi-square test for R %f\n", Chi_R);
    printf("Chi-square test for G este %f\n", Chi_G);
    printf("Chi-square test for B este %f\n\n", Chi_B);
}
