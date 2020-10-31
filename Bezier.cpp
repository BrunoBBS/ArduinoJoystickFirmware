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

Point Bezier::eval(int t)
{
    Point ab = Bezier::linerp(this->A, this->B, t);
    Point bc = Bezier::linerp(this->B, this->C, t);
    Point cd = Bezier::linerp(this->C, this->D, t);
    Point abbc = Bezier::linerp(ab, bc, t);
    Point bccd = Bezier::linerp(bc, cd, t);
    Point ret = Bezier::linerp(abbc, bccd, t);
    return ret;
}