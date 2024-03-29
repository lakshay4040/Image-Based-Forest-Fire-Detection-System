#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// pixel is declared global as it is going to be used by the whole program.
long int pixels = 0;

// including all three models for fire detection.
#include "model_YCbCr.h"
#include "model_CIELAB.h"
#include "model_RGB.h"


int main()
{
    // bag variables for BAGGING (Majority Rule).
    int width, height, channels, bagcount_rgb = 0, bagcount_ycbcr = 0, bagcount_cie = 0, bagcount = 0;
    char state[15];

    // final values that are to be compared with their respective thresholds.
    long int finalcount_ycbcr, finalcount_cie, finalcount_rgb;

    // loading the image with the help of stbi_load function into a pointer variable.
    unsigned char *img = stbi_load("images/nfimg/withoutfire.jpg", &width, &height, &channels, 0);   // 0 to load image as it is.
    
    // in case of any exception (SANITY CHECK).ut
    if (img == NULL)
    {
        printf("Error loading the image...\n");
        exit(1);
    }

    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    // allocating space for a new image which is going to be created by the color models.
    size_t img_size = width * height * channels;
    unsigned char *color_img = malloc(img_size);

    // in case of any exception (SANITY CHECK).
    if (color_img == NULL)
    {
        printf("Unable to loacte memory for colored image...\n");
        exit(1);
    }

    unsigned char *color_img_XYZ = malloc(img_size);
    if (color_img_XYZ == NULL)
    {
        printf("Unable to loacte memory for colored image for XYZ...\n");
        exit(1);
    }

    // calculating the total number of pixels in the image.
    for(unsigned char *p = img, *pg = color_img; p != img + img_size; p += channels, pg += channels)
    {
        pixels++;
    }
    printf("Total number of pixels are : %d\n",pixels);
    

    // Final count values of all the models:
    
    // 1. RGB
    finalcount_rgb = model_rgb(img, img_size, color_img, channels, pixels);
    
    // 2. YCbCr
    finalcount_ycbcr = model_ycbcr(img, img_size, color_img, channels);
    
    // 3. CIE LAB
    finalcount_cie = model_cielab(img, img_size, color_img, color_img_XYZ, channels);


    // RGB threshold check:
    double percentRGBinitial = 0, percentRGB = 0;
    percentRGBinitial = finalcount_rgb*100;
    percentRGB = percentRGBinitial/pixels;
    printf("Final count for RGB model is : %d\n", finalcount_rgb);

    //Comparision with Threshold Value
    if (percentRGB > 1.5)
    {
        bagcount_rgb = 1;
    }
    

    // YCbCr threshold check:
    printf("Final count for YCbCr model is : %d\n", finalcount_ycbcr);
    if (finalcount_ycbcr >= 0)
    {
        bagcount_ycbcr = 1;
    }


    // CIE LAB threshold check:
    printf("Final count for CIE LAB model is : %d\n", finalcount_cie);
    double percentinitial, percentcie;
    percentinitial = finalcount_cie*100;
    percentcie = percentinitial/pixels;

    if(percentcie > 1.5)
    {
        bagcount_cie = 1;
    }


    //FINAL VALIDATION:
    bagcount = bagcount_rgb + bagcount_ycbcr + bagcount_cie;
    if(bagcount >=2)
    {
        printf("FINALLY FIRE DETECTED.\n");
        // printf("Enter the state : ");
        // gets(state);
        // printf("STATE: %s\n",state);
    }
    else
    {
        printf("NO FIRE DETECTED.\n");
    }
    

    printf("RGB : %d\n",bagcount_rgb);
    printf("YCbCr : %d\n",bagcount_ycbcr);
    printf("CIE LAB : %d\n",bagcount_cie);



    // Creating a new image of our desired color model.
    // stbi_write_jpg("fire(3)_ycbcr.jpg", width, height, channels, color_img, 100);

    // free used space (GOOD PRACTICE).
    stbi_image_free(img);
    free(color_img);

    return 0;
}