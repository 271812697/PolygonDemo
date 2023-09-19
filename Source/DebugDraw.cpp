#include "DebugDraw.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

DebugDraw maindraw;//让别的模块链接到这个符号
Camera g_camera;
struct GLRenderPoints {
    void create() {
#pragma region 创建着色器程序
        const char* V = \
            "#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "layout(location = 2) in float v_size;\n"
            "out vec4 f_color;\n"
            "void main(void)\n"
            "{\n"
            "	f_color = v_color;\n"
            "	gl_Position = projectionMatrix*vec4(v_position, 0.0f, 1.0f);\n"
            "   gl_PointSize = v_size;\n"
            "}\n";

        const char* F = \
            "#version 330\n"
            "in vec4 f_color;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "	color = f_color;\n"
            "}\n";

        unsigned vP = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vP, 1, &V, NULL);
        unsigned fP = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fP, 1, &F, NULL);

        char infolog[512];
        int success;
        glCompileShader(vP);
        glGetShaderiv(vP, GL_COMPILE_STATUS, &success);
        if (!success) {
            glad_glGetShaderInfoLog(vP, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "顶点着色器编译成功" << std::endl;
        }
        glCompileShader(fP);
        glGetShaderiv(fP, GL_COMPILE_STATUS, &success);
        if (!success) {
            glad_glGetShaderInfoLog(fP, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "片段着色器编译成功" << std::endl;
        }

        shader = glCreateProgram();
        glAttachShader(shader, vP);
        glAttachShader(shader, fP);

        glLinkProgram(shader);
        glGetShaderiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glad_glGetProgramInfoLog(shader, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "着色器链接成功" << std::endl;
        }
        glDeleteShader(vP);
        glDeleteShader(fP);


#pragma endregion

        projection=glGetUniformLocation(shader,"projectionMatrix");
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        glEnableVertexAttribArray(0);//位置属性
        glEnableVertexAttribArray(1);//颜色属性	
        glEnableVertexAttribArray(2);//大小属性
        glGenBuffers(3, bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[0]);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[1]);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[2]);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(size), size, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;


    }
    void Vertex(const Vec2& v, const Color& c, float s) {//在缓冲区中送入一个顶点
        if (m_count == 1024) {	//缓冲区的数据满了
            fflush();
        }
        vertices[m_count] = v;
        color[m_count] = c;
        size[m_count] = s;
        m_count++;
    }
    void fflush() {//实际的绘制操作
        if (m_count == 0)
            return;
        //后面可以加设置投影矩阵的代码


        glUseProgram(shader);
       
         float p[16] = { 0.0 };
         g_camera.BuildProjectionMatrix(p, 0.0);
         glUniformMatrix4fv(projection, 1, GL_FALSE, p);



        glBindVertexArray(vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, bufferId[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vec2), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Color), color);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(float), size);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, m_count);
        glDisable(GL_PROGRAM_POINT_SIZE);
        glBindVertexArray(0);
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_count = 0;
    }
    unsigned int m_count;//点的数目
    unsigned int vaoId;//
    unsigned int bufferId[3];
    unsigned int shader;//着色器程序
    Vec2 vertices[1024];//位置
    Color color[1024];//颜色
    float size[1024];//大小
    int projection;
};
struct GLRenderLines {
    void create() {
#pragma region 创建着色器程序
        const char* V = \
            "#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "out vec4 f_color;\n"
            "void main(void)\n"
            "{\n"
            "	f_color = v_color;\n"
            "	gl_Position =projectionMatrix*vec4(v_position, 0.0f, 1.0f);\n"

            "}\n";

        const char* F = \
            "#version 330\n"
            "in vec4 f_color;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "	color = f_color;\n"
            "}\n";

        unsigned vP = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vP, 1, &V, NULL);
        unsigned fP = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fP, 1, &F, NULL);

        char infolog[512];
        int success;
        glCompileShader(vP);
        glGetShaderiv(vP, GL_COMPILE_STATUS, &success);
        if (!success) {
            glad_glGetShaderInfoLog(vP, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "顶点着色器编译成功" << std::endl;
        }
        glCompileShader(fP);
        glGetShaderiv(fP, GL_COMPILE_STATUS, &success);
        if (!success) {
            glad_glGetShaderInfoLog(fP, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "片段着色器编译成功" << std::endl;
        }

        shader = glCreateProgram();
        glAttachShader(shader, vP);
        glAttachShader(shader, fP);

        glLinkProgram(shader);
        glGetShaderiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glad_glGetProgramInfoLog(shader, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "着色器链接成功" << std::endl;
        }
        glDeleteShader(vP);
        glDeleteShader(fP);


#pragma endregion

        projection = glGetUniformLocation(shader, "projectionMatrix");
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        glEnableVertexAttribArray(0);//位置属性
        glEnableVertexAttribArray(1);//颜色属性	

        glGenBuffers(2, bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[0]);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[1]);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;


    }
    void Vertex(const Vec2& v,const Color& c) {//在缓冲区中送入一个顶点
        if (m_count == 1024) {	//缓冲区的数据满了
            fflush();
        }
        vertices[m_count] = v;
        color[m_count] = c;
        m_count++;
    }
    void fflush() {//实际的绘制操作
        if (m_count == 0)
            return;
        //后面可以加设置投影矩阵的代码


        glUseProgram(shader);

       
        float p[16] = { 0.0 };
        g_camera.BuildProjectionMatrix(p, 0.0);
        glUniformMatrix4fv(projection, 1, GL_FALSE, p);
        glBindVertexArray(vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, bufferId[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vec2), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Color), color);


        glDrawArrays(GL_LINES, 0, m_count);

        glBindVertexArray(0);
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_count = 0;
    }
    unsigned int m_count;//点的数目
    unsigned int vaoId;//
    unsigned int bufferId[2];
    unsigned int shader;//着色器程序
    Vec2 vertices[1024];//位置
    Color color[1024];//颜色
    int projection;
};
struct GLRenderTriangles
{
    void create() {
#pragma region 创建着色器程序
        const char* V = \
            "#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "out vec4 f_color;\n"
            "void main(void)\n"
            "{\n"
            "	f_color = v_color;\n"
            "	gl_Position = projectionMatrix*vec4(v_position, 0.0f, 1.0f);\n"

            "}\n";

        const char* F = \
            "#version 330\n"
            "in vec4 f_color;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "	color = f_color;\n"
            "}\n";

        unsigned vP = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vP, 1, &V, NULL);
        unsigned fP = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fP, 1, &F, NULL);

        char infolog[512];
        int success;
        glCompileShader(vP);
        glGetShaderiv(vP, GL_COMPILE_STATUS, &success);
        if (!success) {
            glad_glGetShaderInfoLog(vP, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "顶点着色器编译成功" << std::endl;
        }
        glCompileShader(fP);
        glGetShaderiv(fP, GL_COMPILE_STATUS, &success);
        if (!success) {
            glad_glGetShaderInfoLog(fP, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "片段着色器编译成功" << std::endl;
        }

        shader = glCreateProgram();
        glAttachShader(shader, vP);
        glAttachShader(shader, fP);

        glLinkProgram(shader);
        glGetShaderiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glad_glGetProgramInfoLog(shader, 512, NULL, infolog);
            std::cout << infolog << std::endl;
        }
        else {
            std::cout << "着色器链接成功" << std::endl;
        }
        glDeleteShader(vP);
        glDeleteShader(fP);


#pragma endregion

        projection = glGetUniformLocation(shader, "projectionMatrix");
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        glEnableVertexAttribArray(0);//位置属性
        glEnableVertexAttribArray(1);//颜色属性	

        glGenBuffers(2, bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[0]);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[1]);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_DYNAMIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;


    }
    void Vertex(const Vec2& v, const Color& c) {//在缓冲区中送入一个顶点
        if (m_count == maxCount) {	//缓冲区的数据满了
            fflush();
        }
        vertices[m_count] = v;
        color[m_count] = c;

        m_count++;
    }
    void fflush() {//实际的绘制操作
        if (m_count == 0)
            return;
        //后面可以加设置投影矩阵的代码


        glUseProgram(shader);
        float p[16] = { 0.0 };
        g_camera.BuildProjectionMatrix(p, 0.0);
        glUniformMatrix4fv(projection, 1, GL_FALSE, p);
        glBindVertexArray(vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, bufferId[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vec2), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Color), color);



        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, m_count);
        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_count = 0;
    }
    unsigned int m_count;//点的数目
    unsigned int vaoId;//
    unsigned int bufferId[2];
    unsigned int shader;//着色器程序
    static const int maxCount = 1024 * 32;
    Vec2 vertices[maxCount];//位置
    Color color[maxCount];//颜色}drawtriangles;
    int projection;
};

DebugDraw::DebugDraw()
{
    m_lines = nullptr;
    m_triangles = nullptr;
    m_points = nullptr;
}

DebugDraw::~DebugDraw()
{
}

void DebugDraw::Create()
{
    this->m_points = new GLRenderPoints();
    this->m_lines = new GLRenderLines();
    this->m_triangles = new GLRenderTriangles();
    this->m_points->create();
    this->m_lines->create();
    this->m_triangles->create();
}

void DebugDraw::Destroy()
{
    if(m_points!=nullptr)
    delete m_points;
    m_points = nullptr;
    if(m_lines!=nullptr)
    delete m_lines;
    m_lines = nullptr;
    if(m_triangles!=nullptr)
    delete m_triangles;
    m_triangles = nullptr;
}

void DebugDraw::DrawPolygon(const Vec2* vertices, int vertexCount, const Color& color)
{
   const Vec2* p2 = vertices+(vertexCount - 1);
   for (int i = 0; i < vertexCount; i++) {
       m_lines->Vertex(*(vertices+i),color);
       m_lines->Vertex(*p2, color);
       p2 = vertices + i;
   }
}

void DebugDraw::DrawSolidPolygon(const Vec2* vertices, int vertexCount, const Color& color)
{
    Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    for (int i = 1; i < vertexCount - 1; i++) {
        m_triangles->Vertex(*vertices,fillColor);
        m_triangles->Vertex(*(vertices+i), fillColor);
        m_triangles->Vertex(*(vertices+i+1), fillColor);
    }

    const Vec2* p2 = vertices + (vertexCount - 1);
    for (int i = 0; i < vertexCount; i++) {
        m_lines->Vertex(*(vertices + i), color);
        m_lines->Vertex(*p2, color);
        p2 = vertices + i;
    }
}

void DebugDraw::DrawCircle(const Vec2& center, float radius, const Color& color)
{
    float d = 2 * 3.14159265359f /32;
    float sind = sinf(d);
    float cosd = cosf(d);
    Vec2 start = radius * Vec2(0.0, 1.0);
    for (int i = 0; i < 32; i++) {
        Vec2 next = {cosd*start.x-sind*start.y,sind*start.x+cosd*start.y};
        m_lines->Vertex(center+start,color);
        m_lines->Vertex(center+next, color);
        start = next;
    }
}

void DebugDraw::DrawSolidCircle(const Vec2& center, float radius, const Vec2& axis, const Color& color)
{
    float d = 2 * 3.14159265359f / 32;
    float sind = sinf(d);
    float cosd = cosf(d);
    Vec2 start = radius * Vec2(0.0, 1.0);
    Color c = {color.r/2,color.g/2,color.b/2,color.a/2};
    for (int i = 0; i < 32; i++) {
        Vec2 next = { cosd * start.x - sind * start.y,sind * start.x + cosd * start.y };
        m_triangles->Vertex(center + start, c);
        m_triangles->Vertex(center + next, c);
        m_triangles->Vertex(center,c);
        start = next;
    }
    DrawCircle(center,radius,color);
    m_lines->Vertex(center,color);
    m_lines->Vertex(center + radius*axis, color);
}

void DebugDraw::DrawSegment(const Vec2& p1, const Vec2& p2, const Color& color)
{
    m_lines->Vertex(p1,color);
    m_lines->Vertex(p2,color);
}


void DebugDraw::DrawAABB(AABB* aabb, const Color& color)
{
    m_lines->Vertex(aabb->lowerBound,color);
    m_lines->Vertex(Vec2(aabb->lowerBound.x,aabb->upperBound.y),color);

    m_lines->Vertex(Vec2(aabb->lowerBound.x, aabb->upperBound.y), color);
    m_lines->Vertex(aabb->upperBound,color);

    m_lines->Vertex(aabb->upperBound, color);
    m_lines->Vertex(Vec2(aabb->upperBound.x,aabb->lowerBound.y ), color);

    m_lines->Vertex(Vec2(aabb->upperBound.x, aabb->lowerBound.y), color);
    m_lines->Vertex(aabb->lowerBound, color);


}

void DebugDraw::Flush()
{
    m_lines->fflush();
    m_triangles->fflush();
    m_points->fflush();
}

Camera::Camera()
{
    m_width = 1280;
    m_height = 800;
    ResetView();
}

void Camera::ResetView()
{
    m_center={0.0f, 20.0f};
    m_zoom = 1.0f;
}
//相机上的屏幕对应于世界空间的一块矩形区域
Vec2 Camera::ConvertScreenToWorld(const Vec2& ps)
{

    float w = float(m_width);
    float h = float(m_height);
    float u = ps.x / w;
    float v = (h - ps.y) / h;

    float ratio = w / h;
    Vec2 extents(ratio * 25.0f, 25.0f);
    extents *= m_zoom;

    Vec2 lower = m_center - extents;
    Vec2 upper = m_center + extents;

    Vec2 pw;
    pw.x = (1.0f - u) * lower.x + u * upper.x;//插值算法
    pw.y = (1.0f - v) * lower.y + v * upper.y;
    return pw;
}

Vec2 Camera::ConvertWorldToScreen(const Vec2& pw)
{

    float w = float(m_width);
    float h = float(m_height);
    float ratio = w / h;
    Vec2 extents(ratio * 25.0f, 25.0f);
    extents *= m_zoom;

    Vec2 lower = m_center - extents;
    Vec2 upper = m_center + extents;

    float u = (pw.x - lower.x) / (upper.x - lower.x);
    float v = (pw.y - lower.y) / (upper.y - lower.y);

    Vec2 ps;
    ps.x = u * w;
    ps.y = (1.0f - v) * h;
    return ps;
}

void Camera::BuildProjectionMatrix(float* m, float zBias)
{
    float w = float(m_width);
    float h = float(m_height);
    float ratio = w / h;
    Vec2 extents(ratio * 25.0f,25.0f);
    extents *= m_zoom;

    Vec2 lower = m_center - extents;
    Vec2 upper = m_center + extents;

    m[0] = 2.0f / (upper.x - lower.x);
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;

    m[4] = 0.0f;
    m[5] = 2.0f / (upper.y - lower.y);
    m[6] = 0.0f;
    m[7] = 0.0f;

    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 1.0f;
    m[11] = 0.0f;

    m[12] = -(upper.x + lower.x) / (upper.x - lower.x);
    m[13] = -(upper.y + lower.y) / (upper.y - lower.y);
    m[14] = zBias;
    m[15] = 1.0f;
}
