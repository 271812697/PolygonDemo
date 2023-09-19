

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"DebugDraw.h"
#include<math.h>
#include "Vertex.h"
#include "Polygon.h"
#include"Dynamic_Tree.h"
#include<vector>
#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#pragma region 多边形相交问题

struct Polygon poly1, poly2;
DynamicTree* tree=nullptr;
#pragma endregion

bool rightMouseDown = false;
bool leftMouseDown = false;
Vertex clickPointWS;
std::vector<Vertex*>v_capture;
void g_mainWindow(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
    g_camera.m_width = w;
    g_camera.m_height = h;
}
void Scorllcallback(GLFWwindow* window, double dx, double dy) {
    if (dy > 0)
    {
        g_camera.m_zoom /= 1.1f;
    }
    else
    {
        g_camera.m_zoom *= 1.1f;
    }
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            // Quit
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_LEFT:
            // Pan left 
            g_camera.m_center.x -= 0.5f;
            break;
        case GLFW_KEY_RIGHT:
            // Pan left 
            g_camera.m_center.x += 0.5f;
            break;
        case GLFW_KEY_UP:
            g_camera.m_center.y += 0.5f;
            break;
        case GLFW_KEY_DOWN:
            g_camera.m_center.y -= 0.5f;
            break;
        default:break;
        }
    }
}
void mouseButton(GLFWwindow* window, int B, int S, int F) {
    double xd, yd;
    glfwGetCursorPos(window, &xd, &yd);
    Vec2 ps((float)xd, (float)yd);
    if (B == GLFW_MOUSE_BUTTON_2) {//鼠标右键回调事件
        if (S == GLFW_PRESS) {
            rightMouseDown = true;
            clickPointWS = g_camera.ConvertScreenToWorld(ps);
        }
        if (S == GLFW_RELEASE) {
            rightMouseDown = false;
        }
    }
    if (B == GLFW_MOUSE_BUTTON_1) {//鼠标左键回调事件
        if (S == GLFW_PRESS) {
            leftMouseDown = true;
            clickPointWS = g_camera.ConvertScreenToWorld(ps);
            for (auto& it : poly1.m_vertices) {
                float deltax = it.x - clickPointWS.x;
                float deltay = it.y - clickPointWS.y;
                float delta = deltax * deltax + deltay * deltay;
                if (delta < 0.1* g_camera.m_zoom) {
                    v_capture.push_back(&it);
                }
            }
            for (auto& it : poly2.m_vertices) {
                float deltax = it.x - clickPointWS.x;
                float deltay = it.y - clickPointWS.y;
                float delta = deltax * deltax + deltay * deltay;
                if (delta < 0.1* g_camera.m_zoom) {
                    v_capture.push_back(&it);
                }
            }
        }
        if (S == GLFW_RELEASE) {
            leftMouseDown = false;
            v_capture.clear();
        }
    }
}
void mouseMove(GLFWwindow* window, double x, double y) {
    Vec2 ps((float)x, (float)y);
    Vec2 pw = g_camera.ConvertScreenToWorld(ps);

    if (rightMouseDown)
    {
        Vec2 diff = pw - clickPointWS;
        g_camera.m_center.x -= diff.x;
        g_camera.m_center.y -= diff.y;
    }
    if (leftMouseDown)
    {
        Vec2 diff = pw - clickPointWS;
        for (auto it : v_capture) {
            it->x += diff.x;
            it->y += diff.y;
        }
    }
    clickPointWS = g_camera.ConvertScreenToWorld(ps);
}

void Draw() {
    if (tree!=nullptr) {
        delete tree;
    }
    tree = new DynamicTree();
    tree->CreateProxy(poly1.GetAABB(),&poly1);
    tree->CreateProxy(poly2.GetAABB(),&poly2);
    std::vector<struct Polygon> ans;
    tree->Query(ans, poly1);
    //std::vector<struct Polygon> ans= IntersectionPolygons(poly1, poly2);
    for (auto& it : ans) {
        for (auto it1 : it.m_vertices) {
            maindraw.DrawSolidCircle({ it1.x,it1.y }, 0.5, { 0,0 }, { 1.0,0.0,0.0,1.0 });
        }
        maindraw.DrawSolidPolygon((Vec2*)it.m_vertices.data(), it.m_vertices.size(), { 1.0,0.0,1.0,1.0 });
        tree->CreateProxy(it.GetAABB(), nullptr);
    }

    auto p1 = DividePolygonFromOrderVertexs(poly1.m_vertices);
    auto p2 = DividePolygonFromOrderVertexs(poly2.m_vertices);

    for (auto it : p1) {

        maindraw.DrawSolidPolygon((Vec2*)it.data(), it.size(), { 1.0,1.0,0.0,1.0 });
    }
    for (auto it : p2) {
        maindraw.DrawSolidPolygon((Vec2*)it.data(), it.size(), { 0.0,1.0,1.0,1.0 });
    }

    for (auto box : tree->GetAllAABB()) {
      maindraw.DrawAABB((AABB*)(&box), {0.0,0.0,1.0,1.0});
    }
    maindraw.Flush();
}

void mycode(GLFWwindow* window) {

    poly1.m_vertices = { {10,10} ,{20,10},{20,20},{10,20} };
    poly2.m_vertices = { {0,0} ,{-10,-10},{15,0},{-10,10},{-20,10} };

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Text("Hello, world %d", 123);
        if (ImGui::Button("Save"))
        {


        }

        char buf[10] = "name";
        float f;
        ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(g_camera.m_width, g_camera.m_height, "Polygon test", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetWindowSizeCallback(window, g_mainWindow);
    glfwSetScrollCallback(window, Scorllcallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, mouseMove);

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glEnable(GL_DEPTH_TEST);
    maindraw.Create();
    mycode(window);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

