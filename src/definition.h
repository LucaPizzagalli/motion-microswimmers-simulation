#define SCREEN_HEIGHT 400
#define SCREEN_WIDTH 400

#ifndef CAMERA_H
#define CAMERA_H

struct Camera
{
    unsigned char pixels[SCREEN_HEIGHT][SCREEN_WIDTH][4];
    double x;
    double y;
    double zoom;
};

#endif