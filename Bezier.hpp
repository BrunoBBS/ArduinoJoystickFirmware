class Point {
public:
    int x, y;
    Point();
    Point(int x, int y);
    Point operator=(const Point& a)
    {
        this->x = a.x;
        this->y = a.y;
        return *this;
    }
    Point operator+(const Point& a)
    {
        this->x += a.x;
        this->y += a.y;
        return *this;
    }
    Point operator-(const Point& a)
    {
        this->x -= a.x;
        this->y -= a.y;
        return *this;
    }
    Point operator*(const int& a)
    {
        this->x *= a;
        this->y *= a;
        return *this;
    }
    Point operator*(const float& a)
    {
        this->x *= a;
        this->y *= a;
        return *this;
    }
    Point operator/(const float& a)
    {
        this->x /= a;
        this->y /= a;
        return *this;
    }
    Point operator>>(const int& a)
    {
        this->x >>= a;
        this->y >>= a;
        return *this;
    }
};

// bezier curve library to be used specifically with integers between 0 and 1023
class Bezier {
public:
    Point A, B, C, D;

    Bezier() = default;

    Bezier(Point A, Point B, Point C, Point D);

    Point eval(int t);

    static Point linerp(Point a, Point b, int t);
};