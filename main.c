#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encryption.h"
#include "template.h"

int main()
{
    char *encryptedIMG = (char *) malloc(25);
    printf("Name of the encrypted image: ");
    scanf("%s", encryptedIMG);
    printf("\n");

    char *image = (char *) malloc(25);
    printf("Name of the image to be encrypted: ");
    scanf("%s", image);
    printf("\n");

    char *keyFile = (char *) malloc(25);
    printf("Name of the secret key file: ");
    scanf("%s", keyFile);
    printf("\n");

    char *decrytedIMG = (char *) malloc(25);
    printf("Name of the image to be decrypted: ");
    scanf("%s", decrytedIMG);
    printf("\n");

    
    PIXEL* linearIMG = linear_image(image);


    encryption(image, encryptedIMG, keyFile);
    decryption(image, encryptedIMG, decrytedIMG, keyFile);
    Chi2Test(image);
    Chi2Test(encryptedIMG);

    free(linearIMG);

    char *newImage = (char*) "test_i.bmp";
    double ps = 0.5;

    template(image, newImage, ps);

   
    return 0;
}
