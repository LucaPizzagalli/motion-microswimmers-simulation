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

    CellForce(double body_x = 0., double body_y = 0., double flagella_x = 0., double flagella_y = 0.)
        : body_x(body_x), body_y(body_y), flagella_x(flagella_x), flagella_y(flagella_y)
    {}

    CellForce operator+(const CellForce &other) const
    {
        return CellForce(body_x + other.body_x, body_y + other.body_y, flagella_x + other.flagella_x, flagella_y + other.flagella_y);
    }
    void operator+=(const CellForce &other)
    {
        body_x += other.body_x;
        body_y += other.body_y;
        flagella_x += other.flagella_x;
        flagella_y += other.flagella_y;
    }
};
struct Camera
{
    unsigned char pixels[SCREEN_HEIGHT][SCREEN_WIDTH][4];
    double x;
    double y;
    double zoom;
};

#endif