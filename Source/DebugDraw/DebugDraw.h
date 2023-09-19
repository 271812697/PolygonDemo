#pragma once
#include"../Polygon/Vertex.h"

struct GLRenderPoints;
struct GLRenderLines;
struct GLRenderTriangles;

struct Color
{
	Color() {}
	Color(float rIn, float gIn, float bIn, float aIn = 1.0f)
	{
		r = rIn; g = gIn; b = bIn; a = aIn;
	}

	void Set(float rIn, float gIn, float bIn, float aIn = 1.0f)
	{
		r = rIn; g = gIn; b = bIn; a = aIn;
	}

	float r, g, b, a;
};
struct Camera
{
	Camera();

	void ResetView();
	Vec2 ConvertScreenToWorld(const Vec2& screenPoint);
	Vec2 ConvertWorldToScreen(const Vec2& worldPoint);
	void BuildProjectionMatrix(float* m, float zBias);

	Vec2 m_center;
	float m_zoom;
	int m_width;
	int m_height;
};

class DebugDraw 
{
public:
	
	DebugDraw();
	~DebugDraw();

	void Create();
	void Destroy();

	void DrawPolygon(const Vec2* vertices, int vertexCount, const Color& color) ;

	void DrawSolidPolygon(const Vec2* vertices, int vertexCount, const Color& color) ;

	void DrawCircle(const Vec2& center, float radius, const Color& color) ;
	
	
	

	void DrawSolidCircle(const Vec2& center, float radius, const Vec2& axis, const Color& color) ;

	void DrawSegment(const Vec2& p1, const Vec2& p2, const Color& color) ;



	void DrawAABB(AABB* aabb, const Color& color);

	void Flush();
private:
	 GLRenderPoints* m_points;
	 GLRenderLines* m_lines;
	 GLRenderTriangles* m_triangles;

};

extern DebugDraw maindraw;
extern Camera g_camera;