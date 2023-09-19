#include<iostream>
#include"Polygon.h"
/*
* 判断点是否在多边形内
* v 顶点
* lines 多边形的边
*
*/

bool VertexInPolygon(Vertex v, std::vector<LineSegment>lines) {
    float x = v.x;
    float y = v.y;
    int cnt = 0;
    for (auto& line : lines) {
        //点刚好在多边形的边上
        if (Intersection(v, line)) {
            return true;
        }
        float minY = Min(line.p1.y, line.p2.y);
        float maxY = Max(line.p1.y, line.p2.y);
        //去掉最大的端点，可以处理两天线段共交点的情况
        if (y >= minY && y < maxY) {
            float interX = (y - line.p1.y) * (line.p2.x - line.p1.x) / (line.p2.y - line.p1.y) + line.p1.x;
            if (x > interX) {
                cnt++;
            }
        }
    }
    return cnt % 2 != 0;
}


/*
* 求一个多边形在另一个多边形里面所有的顶点
* polygon 检测的多边形
* lines 另一个多边形的边
* 点射法判断一个多边形所有的顶点是否在另一个多边形内，并返回所有的结果
*
*/

std::vector<Vertex> PolygonInPolygon(struct Polygon polygon, std::vector<LineSegment>lines) {
    std::vector<Vertex>ans;
    for (auto& v : polygon.m_vertices) {
        if (VertexInPolygon(v, lines)) {
            ans.push_back(v);
        }
    }
    return ans;
}


/*
* 判断顶点a、b分别与center构成的夹角
*/

bool VertexCmp(const Vertex& a, const Vertex& b, const Vertex& center) {

    //向量OA和向量OB的叉积
    int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
    float angle_a = atan2(a.y - center.y, a.x - center.x);
    float angle_b = atan2(b.y - center.y, b.x - center.x);
    return angle_a < angle_b;
}


/*
* 返回两个凸多边形的交集，
* polygon1，polygon2均为凸多边形
* 求两个多边形的线段的交点，求包含在另一个多边形里面的顶点
* 收集完毕后排序，返回交集的多边形
*
*/

std::vector<Vertex> IntersectionPolygon(struct Polygon polygon1, struct Polygon polygon2) {
    std::vector<LineSegment>line1 = polygon1.Edges();
    std::vector<LineSegment>line2 = polygon2.Edges();
    //如果两个多边形存在线段相交，则返回相交
    std::vector<Vertex> ans = IntersectionPloygonLines(line1, line2);

    //判断两个多边形的互相包含的点
    std::vector<Vertex> l1 = PolygonInPolygon(polygon1, line2);
    std::vector<Vertex> l2 = PolygonInPolygon(polygon2, line1);
    ans.insert(ans.end(), l1.begin(), l1.end());
    ans.insert(ans.end(), l2.begin(), l2.end());
    //冒泡排序
    if (ans.size() != 0) {
        Vertex center = { 0,0 };
        float x = 0, y = 0;
        for (int i = 0; i < ans.size(); i++) {
            x += ans[i].x;
            y += ans[i].y;
        }
        center.x = x / (ans.size());
        center.y = y / (ans.size());
        for (int i = 0; i < ans.size(); i++) {
            for (int j = 0; j < ans.size() - 1; j++) {
                if (VertexCmp(ans[j], ans[j + 1], center)) {
                    Vertex tmp = ans[j];
                    ans[j] = ans[j + 1];
                    ans[j + 1] = tmp;
                }
            }
        }
    }
    return ans;
}


/*
* 判断多边形的走向是顺时针还是逆时针
*/

bool PolygonIsAntiClockwise(std::vector<Vertex>& vertices) {
    int n = vertices.size();
    //计算多边形面积
    if (n < 3)
        return true;
    float s = vertices[0].y * (vertices[n - 1].x - vertices[1].x);
    for (int i = 1; i < n; ++i)
        s += vertices[i].y * (vertices[i - 1].x - vertices[((i + 1) >= n ? 0 : (i + 1))].x);
    s = s * 0.5;
    return s > 0.f;
}


/*
* 得到多边形首尾相连的向量
*/

std::vector<Vertex> GetVectorArrByPointsArr(const std::vector<Vertex> _points)
{
    std::vector<Vertex> t_res;
    int t_pointsNum = _points.size();
    if (t_pointsNum > 1) {
        Vertex t_p1;
        Vertex t_p2;
        for (int i = 0; i < _points.size(); i++) {
            t_p1 = _points[i];
            if (i == t_pointsNum - 1) {
                t_p2 = _points[0];
            }
            else {
                t_p2 = _points[i + 1];
            }
            t_res.push_back(t_p2 - t_p1);
        }
    }
    return t_res;
}


/*
* 判断两条线段是否相交，若相交返回想交点
*/

bool LineIntersect3D(const Vertex& line1P1, const Vertex& line1P2, const Vertex& line2P1, const Vertex& line2P2, Vertex& intersectP)
{
    Vertex v1 = line1P2 - line1P1;
    Vertex v2 = line2P2 - line2P1;

    if (abs(v1.x * v2.y - v1.y * v2.x) < 0.0001) {
        // 两线平行
        return false;
    }

    Vertex startPointSeg = line2P1 - line1P1;
    float vecS1 = v1.x * v2.y - v1.y * v2.x;
    float vecS2 = startPointSeg.x * v2.y - startPointSeg.y * v2.x;

    // 有向面积比值，利用点乘是因为结果可能是正数或者负数　　　　　　　　　　　　　　　　
    if (abs(vecS1 * vecS1) < 1e-5f) {
        return false;
    }
    float num2 = vecS2 / vecS1;
    intersectP = line1P1 + num2 * v1;
    Vertex v11 = intersectP - line2P1;
    //是否在线段2延长线上
    if ((v11.Size() / v2.Size()) > 1)
        return false;
    return true;
}


/*
* 按照索引范围收集多边形的顶点，用于凹多边形分割。
*
*/

std::vector<Vertex> GetPointsByIndexRange(const std::vector<Vertex> _points, int startIndex, int endIndex)
{
    std::vector<Vertex> pts;
    if (startIndex <= endIndex) {
        int idx = startIndex;
        while (idx <= endIndex && idx < _points.size()) {
            pts.push_back(_points[idx]);
            idx++;
        }
    }
    else {
        int idx = startIndex;
        while (idx < _points.size()) {
            pts.push_back(_points[idx]);
            idx++;
        }
        idx = 0;
        while (idx <= endIndex && idx < _points.size()) {
            pts.push_back(_points[idx]);
            idx++;
        }
    }
    return pts;
}


/*
*对凹多边形进行分割，利用找到的第一个凹向量延长与多边形求交点，将多边形划分两部分
*/

bool GetRayIntersectionOfVecInVecArr(const std::vector<Vertex> _dirs, const std::vector<Vertex> _points,
    const int _vecIndex, const int _beginIndex, const int _endIndex, std::vector<Vertex>& _dividePolygonA,
    std::vector<Vertex>& _dividePolygonB)
{
    int t_dirsNum = _dirs.size(); //向量个数
    int t_pointsNum = _points.size(); //点的个数

    if (t_dirsNum > 3 && t_pointsNum > 3) {
        if (t_dirsNum == t_pointsNum) {
            if (_beginIndex >= 0 && _beginIndex < t_dirsNum) {
                if (_endIndex >= 0 && _endIndex < t_dirsNum) {
                    int t_indexNew; //向量头对应点的下标
                    if (_vecIndex == (t_dirsNum - 1)) //为向量组最后一个向量
                        t_indexNew = 0;
                    else
                        t_indexNew = _vecIndex + 1;

                    Vertex t_beginA = _points[_vecIndex]; //凹处第一个向量起点
                    float k = 1e14;
                    Vertex ExtendedVector = k * _dirs[_vecIndex];
                    Vertex t_endA = t_beginA + ExtendedVector; //用线段代替射线

                    Vertex t_intersectionPoint;
                    for (int j = _beginIndex; j <= _endIndex; j++) {
                        if (j != _vecIndex && j != t_indexNew) { //不是向量_vecIndex的端点
                            Vertex t_beginB = _points[j];
                            if (LineIntersect3D(t_beginA, t_endA, t_beginB, t_beginB + _dirs[j], t_intersectionPoint)) {
                                //给分割的多边形点组加点
                                std::cout << "凹点向量下标: " << _vecIndex << ",  相交向量下标: " << j << std::endl;

                                _dividePolygonA = GetPointsByIndexRange(_points, t_indexNew, j);
                                _dividePolygonA.push_back(t_intersectionPoint);

                                std::cout << "_dividePolygonA向量数组个数： " << _dividePolygonA.size() << std::endl;
                                _dividePolygonB = GetPointsByIndexRange(_points, j, t_indexNew - 1);
                                if (_dividePolygonB.size() > 0) {
                                    _dividePolygonB[0] = t_intersectionPoint;
                                }
                                std::cout << "_dividePolygonB向量数组个数： " << _dividePolygonB.size() << std::endl;

                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

/*
* 将凹多边形分割成凸多边形
* 先检查是否是凹多边形，若是则进行分割操作
*/

std::vector<std::vector<Vertex>> DividePolygonIfConcave(std::vector<Vertex> _points, bool _antiClockwise)
{
    std::vector<std::vector<Vertex>> polygonDividedArr;
    int t_pointsNum = _points.size();
    if (t_pointsNum < 3) {
        return polygonDividedArr;
    }
    else if (t_pointsNum == 3) {
        polygonDividedArr.push_back(_points);
        return polygonDividedArr;
    }
    else if (t_pointsNum > 3) {
        std::vector<Vertex> _dividePolygonA;
        std::vector<Vertex> _dividePolygonB;
        Vertex t_p1;
        Vertex t_p2;
        // 将点集转换为首尾相接的向量集
        std::vector<Vertex> t_dirs = GetVectorArrByPointsArr(_points);

        bool t_divideResult = false;
        //后一个向量下标，也是前一个向量终点的下标
        int t_indexNew = 0;
        for (int i = 0; i < t_dirs.size(); i++) {
            t_p1 = t_dirs[i];
            if (i == t_dirs.size() - 1) {
                t_p2 = t_dirs[0];
                t_indexNew = 0;
            }
            else {
                t_p2 = t_dirs[i + 1];
                t_indexNew = i + 1;
            }

            double t_rotateDir = (t_p1.x * t_p2.y - t_p1.y * t_p2.x);
            //检查出是凹多边形
            if ((t_rotateDir < -0.0f && _antiClockwise == true) || (t_rotateDir > 0.0f && _antiClockwise == false)) {
                std::cout << "凹多边形" << std::endl;
                //求分割点
                t_divideResult = GetRayIntersectionOfVecInVecArr(t_dirs, _points, i, i, t_pointsNum - 1, _dividePolygonA, _dividePolygonB);
                if (t_divideResult == false) {
                    t_divideResult = GetRayIntersectionOfVecInVecArr(t_dirs, _points, i, 0, i - 1, _dividePolygonA, _dividePolygonB);
                }
                if (t_divideResult == false) {
                    std::cout << "线段" << i << " 没有得到分割点" << std::endl;
                }
                break;
            }
        }

        if (t_divideResult == false) {
            polygonDividedArr.push_back(_points);
        }
        else {
            if (_dividePolygonA.size() > 2) {
                std::vector<std::vector<Vertex>>temp = DividePolygonIfConcave(_dividePolygonA, _antiClockwise);
                polygonDividedArr.insert(polygonDividedArr.end(), temp.begin(), temp.end());
            }
            if (_dividePolygonB.size() > 2) {
                std::vector<std::vector<Vertex>>temp = DividePolygonIfConcave(_dividePolygonB, _antiClockwise);
                polygonDividedArr.insert(polygonDividedArr.end(), temp.begin(), temp.end());
            }
        }
    }
    return polygonDividedArr;
}

/*
*
* 从多边形的顶点分割出多个多边形，会先检查相应的朝向
*
*/
std::vector<std::vector<Vertex>>DividePolygonFromOrderVertexs(std::vector<Vertex> _points)
{

    std::vector<std::vector<Vertex>> polygonDividedArr;
    if (_points.size() > 3) {
        // 判断多边形是否逆时针
        bool _antiClockwise = PolygonIsAntiClockwise(_points);
        // 开始分割
        polygonDividedArr = DividePolygonIfConcave(_points, _antiClockwise); //递归

        if (polygonDividedArr.size() > 0) {
            std::cout << "分割后多边形个数：" << polygonDividedArr.size() << std::endl;
        }
    }
    else {
        std::cout << "传入图形不是多边形~~~~~~~~~~~" << std::endl;

    }
    return polygonDividedArr;
}

/*
* 返回两个任意多边形的交集，
* polygon1，polygon2均为任意多边形
*
*
*/

bool IntersectionPolygons(struct Polygon& polygon1, struct Polygon& polygon2, std::vector<struct Polygon>&ans) {
    //先对任意多边形进行分割，将凹多边形分割成凸多边形
    auto p1 = DividePolygonFromOrderVertexs(polygon1.m_vertices);
    auto p2 = DividePolygonFromOrderVertexs(polygon2.m_vertices);
    
    for (auto it1 : p1) {
        for (auto it2 : p2) {
            std::vector<Vertex> temp = IntersectionPolygon({ it1 }, { it2 });
            if (temp.size() != 0) {
                ans.push_back({ temp });
            }
        }
    }
    
    return ans.size()!=0;
}


