#ifndef DEFINITION_H
#define DEFINITION_H

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 600

#define SQRT_2 1.41421

#include <cmath>

struct Vector2D
{
    double coord[2];

    Vector2D(double x = 0., double y = 0.)
    {
        coord[0] = x;
        coord[1] = y;
    }
    double operator[](const int index) const
    {
        return this->coord[index];
    }
    Vector2D operator+(const Vector2D &other) const
    {
        return Vector2D(this->coord[0] + other[0], this->coord[1] + other[1]);
    }
    Vector2D operator-(const Vector2D &other) const
    {
        return Vector2D(this->coord[0] - other[0], this->coord[1] - other[1]);
    }
    Vector2D operator*(const double &other) const
    {
        return Vector2D(this->coord[0] * other, this->coord[1] * other);
    }
    Vector2D operator/(const double &other) const
    {
        return Vector2D(this->coord[0] / other, this->coord[1] / other);
    }
    void operator+=(const Vector2D &other)
    {
        this->coord[0] += other[0];
        this->coord[1] += other[1];
    }
    void operator-=(const Vector2D &other)
    {
        this->coord[0] -= other[0];
        this->coord[1] -= other[1];
    }
    void operator*=(const double &other)
    {
        this->coord[0] *= other;
        this->coord[1] *= other;
    }
    void operator/=(const double &other)
    {
        this->coord[0] /= other;
        this->coord[1] /= other;
    }
    double operator*(const Vector2D &other) const
    {
        return this->coord[0] * other[0] + this->coord[1] * other[1];
    }
    double cross(Vector2D other) const
    {
        return this->coord[0] * other[1] - this->coord[1] * other[0];
    }
    double square() const
    {
        return this->coord[0] * this->coord[0] + this->coord[1] * this->coord[1];
    }
    double modulus() const
    {
        return sqrt(this->coord[0] * this->coord[0] + this->coord[1] * this->coord[1]);
    }
};

struct Camera
{
    unsigned char pixels[SCREEN_HEIGHT][SCREEN_WIDTH][4];
    Vector2D coord;
    double zoom;
};

#endif