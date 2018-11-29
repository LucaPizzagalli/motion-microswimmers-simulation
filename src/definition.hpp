#ifndef DEFINITION_H
#define DEFINITION_H

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 600

#define SQRT_2 1.41421

struct CellForce
{
    double body_x;
    double body_y;
    double flagella_x;
    double flagella_y;
};
struct Camera
{
    unsigned char pixels[SCREEN_HEIGHT][SCREEN_WIDTH][4];
    double x;
    double y;
    double zoom;
};

#endif