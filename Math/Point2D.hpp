#ifndef MATH_POINT_2D_HPP
#define MATH_POINT_2D_HPP

namespace Math {
    class Point2D
    {
    public:
        Point2D();
        Point2D(float u, float v);

        float u() const;
        float v() const;

        Point2D operator*(float b) const;
        Point2D operator+(const Point2D &b) const;

    private:
        float mU;
        float mV;
    };
}

#endif
