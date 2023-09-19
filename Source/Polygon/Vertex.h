#pragma once
#include<vector>
#include<cmath>


/*
* 2D空间中顶点的定义
* 
*/
struct Vertex {

    Vertex() = default;
    Vertex(float xIn, float yIn) :x(xIn), y(yIn) {}

    float Size() {
        return sqrtf(x * x + y * y);
    }
    float SizeSquared()const {
        return x * x + y * y;
    }
    void operator += (const Vertex& v)
    {
        x += v.x; y += v.y;
    }
    void operator -= (const Vertex& v)
    {
        x -= v.x; y -= v.y;
    }
    void operator *= (float a)
    {
        x *= a; y *= a;
    }

    float x;
    float y;
};
using Vec2 = Vertex;
/*
*  顶点的运算重载操作符
*/
inline Vertex operator + (const Vertex& a, const Vertex& b)
{
    return Vertex(a.x + b.x, a.y + b.y);
}

inline Vertex operator - (const Vertex& a, const Vertex& b)
{
    return Vertex(a.x - b.x, a.y - b.y);
}

inline Vertex operator * (float s, const Vertex& a)
{
    return Vertex(s * a.x, s * a.y);
}

inline bool operator == (const Vertex& a, const Vertex& b)
{
    return a.x == b.x && a.y == b.y;
}

inline bool operator != (const Vertex& a, const Vertex& b)
{
    return a.x != b.x || a.y != b.y;
}
template <typename T>
inline T Min(T a, T b)
{
    return a < b ? a : b;
}

inline Vec2 Min(const Vec2& a, const Vec2& b)
{
    return Vec2(Min(a.x, b.x), Min(a.y, b.y));
}

template <typename T>
inline T Max(T a, T b)
{
    return a > b ? a : b;
}

inline Vec2 Max(const Vec2& a, const Vec2& b)
{
    return Vec2(Max(a.x, b.x), Max(a.y, b.y));
}

/*
* 轴对齐包围盒
*
*/
struct AABB
{
    Vec2 lowerBound;
    Vec2 upperBound;
    bool IsValid() const {
        Vec2 d = upperBound - lowerBound;
        bool valid = d.x >= 0.0f && d.y >= 0.0f;
        return valid;
    }


    Vec2 GetCenter() const
    {
        return 0.5f * (lowerBound + upperBound);
    }

    Vec2 GetExtents() const
    {
        return 0.5f * (upperBound - lowerBound);
    }


    float GetPerimeter() const
    {
        float wx = upperBound.x - lowerBound.x;
        float wy = upperBound.y - lowerBound.y;
        return 2.0f * (wx + wy);
    }


    void Combine(const AABB& aabb)
    {
        lowerBound = Min(lowerBound, aabb.lowerBound);
        upperBound = Max(upperBound, aabb.upperBound);
    }


    void Combine(const AABB& aabb1, const AABB& aabb2)
    {
        lowerBound = Min(aabb1.lowerBound, aabb2.lowerBound);
        upperBound = Max(aabb1.upperBound, aabb2.upperBound);
    }

    /// Does this aabb contain the provided AABB.
    bool Contains(const AABB& aabb) const
    {
        bool result = true;
        result = result && lowerBound.x <= aabb.lowerBound.x;
        result = result && lowerBound.y <= aabb.lowerBound.y;
        result = result && aabb.upperBound.x <= upperBound.x;
        result = result && aabb.upperBound.y <= upperBound.y;
        return result;
    }
};
struct LineSegment {
    Vertex p1;
    Vertex p2;
};

inline bool TestOverlap(const AABB& a, const AABB& b)
{
    Vec2 d1, d2;
    d1 = b.lowerBound - a.upperBound;
    d2 = a.lowerBound - b.upperBound;

    if (d1.x > 0.0f || d1.y > 0.0f)
        return false;

    if (d2.x > 0.0f || d2.y > 0.0f)
        return false;

    return true;
}

/*
* 两个向量的叉乘
* AB
* CD
*/

inline float crossProduct(Vertex A, Vertex B, Vertex C, Vertex D) {
    return (B.x - A.x) * (D.y - C.y) - (B.y - A.y) * (D.x - C.x);
}

/*
*点与线段的相交,若相交则代表点在线段上
*v
*a 起点
*b 终点
*/
inline bool Intersection(Vertex v, Vertex a, Vertex b) {
    float minX = Min(a.x, b.x);
    float maxX = Max(a.x, b.x);
    float minY = Min(a.y, b.y);
    float maxY = Max(a.y, b.y);
    if (v.x > maxX || v.x < minX || v.y > maxY || v.y < minY) {
        return false;
    }
    float res = crossProduct(v, a, v, b);
    return abs(res) < 0.0001;
}

inline bool Intersection(Vertex v, LineSegment line) {
    return Intersection(v, line.p1, line.p2);
}

/*
* 判断两条线段是否有点在对方线段上
*/
std::vector<Vertex> VertexOnLine(LineSegment line1, LineSegment line2);

/*
* 利用叉积判断两条线段给出两条线段的交点
*
*/
std::vector<Vertex> IntersectionLine(LineSegment line1, LineSegment line2);

/*
* 给出两个多边形的线段的所有交点
* 
*/
std::vector<Vertex> IntersectionPloygonLines(std::vector<LineSegment>lines1, std::vector<LineSegment>lines2);
