#include "Bezier.hpp"

Point::Point(int x, int y)
{
    this->x = x;
    this->y = y;
}

Point::Point()
{
    this->x = 0;
    this->y = 0;
}

Bezier::Bezier(Point A, Point B, Point C, Point D)
{
    this->A = A;
    this->B = B;
    this->C = C;
    this->D = D;
}

Point Bezier::linerp(Point a, Point b, int t)
{
    return a + ((b - a) * ((float)t / 1023));
}
