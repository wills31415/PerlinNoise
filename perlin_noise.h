#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "libTableauNoir.h"


/* Déclaration des types et structures. */
typedef struct
{
    double x;
    double y;
} vect2D_t;

typedef struct
{
    size_t dimX;
    size_t dimY;

    vect2D_t** data;
} grid2D_t;

typedef double (*f_interpol1D_t)(double);


/* Déclaration des fonctions. */

/* Fonctions d'allocation et de désallocation d'une grille 2D. */
grid2D_t* grid2D_init(size_t dimX, size_t dimY);
void grid2D_destroy(grid2D_t* grid);

/* Fonction de randomisation unitaire des vecteurs d'une grille 2D. */
void grid2D_randomize_unit(grid2D_t* grid);

/* Fonction de calcul du produit scalaire de deux vecteurs. */
double scalar_prod(vect2D_t u, vect2D_t v);

/* Fonctions d'interpolation 1D. */
double clamping(double x);
double smoothstep(double x);
double smootherstep(double x);

/* Fonction d'interpolation 2D. */
double interpol_2D(double x, double y, double a_00, double a_10, double a_01, double a_11, f_interpol1D_t f);

/* Fonction de calcul du bruit au point de coordonnées (x, y). */
double noise(double x, double y, grid2D_t* grad, f_interpol1D_t f);

/* Fonction de calcul du bruit avec octaves d'après le modèle d'Internet. */
double noise_octaves(double x, double y, size_t nb_octaves, double amplitudes[], grid2D_t* grad, f_interpol1D_t f);
