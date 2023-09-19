#pragma once
#include"Vertex.h"

/*
* 多边形的定义
*/
struct Polygon {
    Polygon() = default;
    std::vector<LineSegment> Edges() {
        std::vector<LineSegment>ans;
        for (int i = 0; i < m_vertices.size() - 1; i++) {
            ans.push_back({ m_vertices[i],m_vertices[i + 1] });
        }
        ans.push_back({ m_vertices[m_vertices.size() - 1],m_vertices[0] });
        return ans;
    }
    AABB GetAABB() {
        Vec2 lower{1e9,1e9}, upper{ -1e9,-1e9 };
        for (Vertex v : m_vertices) {
            lower = Min(v,lower);
            upper = Max(v,upper);
        }
        return AABB{ lower,upper };
    }
    std::vector<Vertex>m_vertices;

};

/*
* 判断点是否在多边形内
* v 顶点
* lines 多边形的边
* 
*/
bool VertexInPolygon(Vertex v, std::vector<LineSegment>lines);

/*
* 求一个多边形在另一个多边形里面所有的顶点
* polygon 检测的多边形
* lines 另一个多边形的边
* 点射法判断一个多边形所有的顶点是否在另一个多边形内，并返回所有的结果
* 
*/

std::vector<Vertex> PolygonInPolygon(struct Polygon polygon, std::vector<LineSegment>lines);



/*
* 返回两个凸多边形的交集，
* polygon1，polygon2均为凸多边形
* 求两个多边形的线段的交点，求包含在另一个多边形里面的顶点
* 收集完毕后排序，返回交集的多边形
* 
*/
std::vector<Vertex> IntersectionPolygon(struct Polygon polygon1, struct Polygon polygon2);

/*
* 判断多边形的走向是顺时针还是逆时针
*/
bool PolygonIsAntiClockwise(std::vector<Vertex>& vertices);


/*
* 得到多边形首尾相连的向量
*/
std::vector<Vertex> GetVectorArrByPointsArr(const std::vector<Vertex> _points);

/*
* 判断两条线段是否相交，若相交返回想交点
*/
bool LineIntersect3D(const Vertex& line1P1, const Vertex& line1P2, const Vertex& line2P1, const Vertex& line2P2, Vertex& intersectP);

/*
* 按照索引范围收集多边形的顶点，用于凹多边形分割。
* 
*/
std::vector<Vertex> GetPointsByIndexRange(const std::vector<Vertex> _points, int startIndex, int endIndex);

/*
*对凹多边形进行分割，利用找到的第一个凹向量延长与多边形求交点，将多边形划分两部分
*/
bool GetRayIntersectionOfVecInVecArr(const std::vector<Vertex> _dirs, const std::vector<Vertex> _points,
    const int _vecIndex, const int _beginIndex, const int _endIndex, std::vector<Vertex>& _dividePolygonA,
    std::vector<Vertex>& _dividePolygonB);

/*
* 将凹多边形分割成凸多边形
* 先检查是否是凹多边形，若是则进行分割操作
*/
std::vector<std::vector<Vertex>> DividePolygonIfConcave(std::vector<Vertex> _points, bool _antiClockwise);

/*
*
* 从多边形的顶点分割出多个多边形，会先检查相应的朝向
* 
*/
std::vector<std::vector<Vertex>>DividePolygonFromOrderVertexs(std::vector<Vertex> _points);

/*
* 返回两个任意多边形的交集，
* polygon1，polygon2均为任意多边形
* 
* 
*/
bool IntersectionPolygons(struct Polygon& polygon1, struct Polygon& polygon2, std::vector<struct Polygon>& ans);

