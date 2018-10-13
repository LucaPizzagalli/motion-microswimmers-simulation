#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 600

#ifndef DEFINITION_H
#define DEFINITION_H

struct Force
{
    double x;
    double y;
};
struct Camera
{
    unsigned char pixels[SCREEN_HEIGHT][SCREEN_WIDTH][4];
    double x;
    double y;
    double zoom;
};

#endif