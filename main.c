#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "libTableauNoir.h"

#include "perlin_noise.h"
#include "PNM.h"



// Déclaration des types et structures utilisateur.


// Déclaration des fonctions utilisateur.



int main(int argc, char** argv)
{
    if(argc < 3) exit(EXIT_FAILURE);
    size_t dimX = (size_t)atoi(argv[1]);
    size_t dimY = (size_t)atoi(argv[2]);

    size_t nb_octaves = 1;
    double* amplitudes = NULL;
    if(argc == 3)
    {
        amplitudes = (double*)malloc(sizeof(double));
        amplitudes[0] = 1;
    }
    else
    {
        nb_octaves = argc - 3;
        amplitudes = (double*)malloc(nb_octaves * sizeof(double));
        for(size_t i = 0; i < nb_octaves; ++i) amplitudes[i] = strtod(argv[3 + i], NULL);
    }

    grid2D_t* grid = grid2D_init(dimX, dimY);
    srand(time(NULL) * clock());
    grid2D_randomize_unit(grid);

    size_t width = 500;
    size_t height = 500;

    PNMImage* output_smoothstep = createPNM(width, height);
    PNMImage* output_smootherstep = createPNM(width, height);

    double x = 0;
    double y = 0;
    unsigned char S, T;

    unsigned char sea_level = 64;
    unsigned char grass_level = 70;
    unsigned char max_height = 120;

    PNMPixel S_pixel;
    PNMPixel T_pixel;

    PNMPixel water = {0, 191, 255};
    PNMPixel sand = {255, 215, 0};
    PNMPixel grass = {0, 128, 0};

    for(size_t i = 0; i < width; ++i)
    {
        x = (double)i / (width - 1);
        for(size_t j = 0; j < height; ++j)
        {
            y = (double)j / (height - 1);
            S = (unsigned char)(max_height * (noise_octaves(x, y, nb_octaves, amplitudes, grid, &smoothstep) + 1) / 2);
            T = (unsigned char)(max_height * (noise_octaves(x, y, nb_octaves, amplitudes, grid, &smootherstep) + 1) / 2);

            if(S <= sea_level) S_pixel = water;
            else if(S >= grass_level) S_pixel = grass;
            else S_pixel = sand;

            if(T <= sea_level) T_pixel = water;
            else if(T >= grass_level) T_pixel = grass;
            else T_pixel = sand;

            output_smoothstep->data[i + j * output_smoothstep->width] = S_pixel;
            output_smootherstep->data[i + j * output_smootherstep->width] = T_pixel;
        }
    }

    writePNM("smoothstep.pnm", output_smoothstep);
    writePNM("smootherstep.pnm", output_smootherstep);

    freePNM(output_smoothstep);
    freePNM(output_smootherstep);
    free(amplitudes);

    grid2D_destroy(grid);

    return EXIT_SUCCESS;
}


// Définition des fonctions utilisateur.
