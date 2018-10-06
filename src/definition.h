#define SCREEN_HEIGHT 400
#define SCREEN_WIDTH 400

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