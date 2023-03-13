#include "perlin_noise.h"


/* Fonction d'allocation d'une grille 2D. */
grid2D_t* grid2D_init(size_t dimX, size_t dimY)
{
    grid2D_t* grid = (grid2D_t*)malloc(sizeof(grid2D_t));

    grid->dimX = dimX;
    grid->dimY = dimY;

    grid->data = (vect2D_t**)malloc((dimX + 1) * sizeof(vect2D_t*));
    for(size_t i = 0; i <= dimX; ++i) grid->data[i] = (vect2D_t*)calloc(dimY + 1, sizeof(vect2D_t));


    return grid;
}


/* Fonction de désallocation d'une grille 2D. */
void grid2D_destroy(grid2D_t* grid)
{
    if(!grid) return;
    if(!(grid->data)) return;

    for(size_t i = 0; i <= grid->dimX; ++i) free(grid->data[i]);
    free(grid->data);

    grid->data = NULL;

    free(grid);
}


/* Fonction de randomisation unitaire des vecteurs d'une grille 2D. */
void grid2D_randomize_unit(grid2D_t* grid)
{
    if(!grid) return;
    if(!(grid->data)) return;

    double r = 0;
    for(size_t i = 0; i < grid->dimX; ++i)
    {
        for(size_t j = 0; j < grid->dimY; ++j)
        {
            r = (double)rand() / RAND_MAX * 2 * M_PI;
            grid->data[i][j] = (vect2D_t){cos(r), sin(r)};
        }
    }
    for(size_t i = 0; i < grid->dimX; ++i) grid->data[i][grid->dimY] = grid->data[i][0];
    for(size_t j = 0; j < grid->dimY; ++j) grid->data[grid->dimX][j] = grid->data[0][j];
    grid->data[grid->dimX][grid->dimY] = grid->data[0][0];
}


/* Fonction de calcul du produit scalaire de deux vecteurs. */
double scalar_prod(vect2D_t u, vect2D_t v)
{
    return u.x * v.x + u.y * v.y;
}


/*Fonctions d'interpolation 1D. */
double clamping(double x)
{
    if(x <= 0) return 0;
    if(x >= 1) return 1;
    return x;
}

double smoothstep(double x)
{
    if(x <= 0) return 0;
    if(x >= 1) return 1;
    return -2 * x * x * (x - 1.5);
}

double smootherstep(double x)
{
    if(x <= 0) return 0;
    if(x >= 1) return 1;
    return x * x * x * (6 * x * x - 15 * x + 10);
}


/* Fonction d'interpolation 2D. */
double interpol_2D(double x, double y, double a_00, double a_10, double a_01, double a_11, f_interpol1D_t f)
{
    if(!f) return 0;
    vect2D_t u = (vect2D_t){f(x), f(y)};
    double A_0 = a_00 + u.x * (a_10 - a_00);
    double A_1 = a_01 + u.x * (a_11 - a_01);
    return A_0 + u.y * (A_1 - A_0);
}


/* Fonction de calcul du bruit au point de coordonnées (x, y). */
double noise(double x, double y, grid2D_t* grad, f_interpol1D_t f)
{
    //if(x < 0 || x > grad->dimX) return 0;
    //if(y < 0 || y > grad->dimY) return 0;

    x = fmod(x, 1) * grad->dimX;
    y = fmod(y, 1) * grad->dimY;

    /* Calcul des coordonnées des points de la grille les plus proches de (x, y). */
    size_t x0 = (size_t)(x < grad->dimX ? x : x - 1);
    size_t x1 = (size_t)(x0 == grad->dimX ? x0 : x0 + 1);
    size_t y0 = (size_t)(y < grad->dimY ? y : y - 1);
    size_t y1 = (size_t)(y0 == grad->dimY ? y0 : y0 + 1);

    /* Calcul des distances du point (x, y) aux bords inférieurs de la cellule. */
    double dx = x - x0;
    double dy = y - y0;

    /* Calcul des valeurs aux nœuds d'interpolation. */
    double a_00 = sqrt(2) * scalar_prod((vect2D_t){x - x0, y - y0}, grad->data[x0][y0]);
    double a_10 = sqrt(2) * scalar_prod((vect2D_t){x - x1, y - y0}, grad->data[x1][y0]);
    double a_01 = sqrt(2) * scalar_prod((vect2D_t){x - x0, y - y1}, grad->data[x0][y1]);
    double a_11 = sqrt(2) * scalar_prod((vect2D_t){x - x1, y - y1}, grad->data[x1][y1]);


    return interpol_2D(dx, dy, a_00, a_10, a_01, a_11, f);
}


/* Fonction de calcul du bruit avec octaves d'après le modèle d'Internet. */
double noise_octaves(double x, double y, size_t nb_octaves, double amplitudes[], grid2D_t* grad, f_interpol1D_t f)
{
    if(!nb_octaves) return 0;
    //if(x < 0 || x > grad->dimX) return 0;
    //if(y < 0 || y > grad->dimY) return 0;

    x = fmod(x, 1);
    y = fmod(y, 1);

    double S = 0;
    double norm = 0;

    for(size_t i = 0; i < nb_octaves; ++i)
    {
        S += amplitudes[i] * noise(x, y, grad, f);
        norm += amplitudes[i];
        //x = (x == grad->dimX ? x : 2 * fmod(x, (double)grad->dimX / 2));
        x = (x == 1 ? x : 2 * fmod(x, 0.5));
        //y = (y == grad->dimY ? y : 2 * fmod(y, (double)grad->dimY / 2));
        y = (y == 1 ? y : 2 * fmod(y, 0.5));
    }


    return S / norm;
}
