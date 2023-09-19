#include"Vertex.h"

/*
* 判断两条线段是否有点在对方线段上
*/

std::vector<Vertex> VertexOnLine(LineSegment line1, LineSegment line2) {
    std::vector<Vertex> ans;

    if (Intersection(line1.p1, line2)) {
        ans.push_back(line1.p1);
    }
    if (Intersection(line1.p2, line2)) {
        ans.push_back(line1.p2);
    }
    if (Intersection(line2.p1, line1)) {
        ans.push_back(line2.p1);
    }
    if (Intersection(line2.p2, line1)) {
        ans.push_back(line2.p2);
    }
    return ans;
}

/*
* 利用叉积判断两条线段是否相交
*
*/

std::vector<Vertex> IntersectionLine(LineSegment line1, LineSegment line2) {
    std::vector<Vertex> ans = VertexOnLine(line1, line2);
    if (ans.size() != 0) {
        return ans;
    }
    float c = crossProduct(line1.p1, line1.p2, line1.p1, line2.p1);
    float d = crossProduct(line1.p1, line1.p2, line1.p1, line2.p2);
    float a = crossProduct(line2.p1, line2.p2, line2.p1, line1.p1);
    float b = crossProduct(line2.p1, line2.p2, line2.p1, line1.p2);
    if (c * d < 0 && a * b < 0) {
        float s = line1.p2.x * line1.p1.y - line1.p2.y * line1.p1.x;
        float t = (line1.p1.y - line1.p2.y) * (line2.p1.x - line2.p2.x) - (line1.p1.x - line1.p2.x) * (line2.p1.y - line2.p2.y);
        float u = line2.p2.x * (line1.p1.y - line1.p2.y) - line2.p2.y * (line1.p1.x - line1.p2.x);
        float k = (s - u) / t;
        Vertex p = { (line2.p1.x - line2.p2.x) * k + line2.p2.x, (line2.p1.y - line2.p2.y) * k + line2.p2.y };
        ans.push_back(p);
    }
    return ans;
}
/*
* 判断两个多边形的线段的所有交点
* 
*/
std::vector<Vertex> IntersectionPloygonLines(std::vector<LineSegment>lines1, std::vector<LineSegment>lines2) {
    std::vector<Vertex>ans;
    for (auto& l1 : lines1) {
        for (auto& l2 : lines2) {
            std::vector<Vertex>temp = IntersectionLine(l1, l2);
            ans.insert(ans.end(), temp.begin(), temp.end());
        }
    }
    return ans;
}

