#pragma once
#include <assert.h>
#include<stack>
#include"Vertex.h"
#include"Polygon.h"

struct  TreeNode
{
    bool IsLeaf() const
    {
        return child1 == -1;
    }
    AABB aabb;
    //叶节点时，所管理的多边形指针
    struct Polygon* userData;

    //树上的节点使用parent，未分配的节点使用next
    union
    {
        int parent;
        int next;
    };

    int child1;
    int child2;
    // leaf = 0, free node = -1
    int height;
};
class DynamicTree
{
public:

    DynamicTree();

    ~DynamicTree();

    //为一个多边形创建一个代理节点，将其插入到树中成为叶子节点
    int CreateProxy(const AABB& aabb, struct Polygon* userData);

    void DestroyProxy(int proxyId);

    void* GetUserData(int proxyId) const;

    //查询树中一个具体节点的AABB
    const AABB& GetFatAABB(int proxyId) const;

    //获得平衡二叉树的所有节点的AABB
    std::vector<AABB> GetAllAABB();

    //查找场景中所有与polygn相交的多边形，将相交结果放在ans中
    bool Query(std::vector<struct Polygon>& ans,  struct Polygon& ploygn) ;

private:

    int AllocateNode();
    void FreeNode(int node);
    void InsertLeaf(int node);
    void RemoveLeaf(int node);
    int Balance(int index);
    int m_root;
    TreeNode* m_nodes;
    int m_nodeCount;
    int m_nodeCapacity;
    int m_freeList;
    int m_insertionCount;
};

inline void* DynamicTree::GetUserData(int proxyId) const
{
    assert(0 <= proxyId && proxyId < m_nodeCapacity);
    return m_nodes[proxyId].userData;
}

inline const AABB& DynamicTree::GetFatAABB(int proxyId) const
{
    assert(0 <= proxyId && proxyId < m_nodeCapacity);
    return m_nodes[proxyId].aabb;
}

inline std::vector<AABB> DynamicTree::GetAllAABB()
{
    std::vector<AABB>ans;
    for (int i = 0; i < m_nodeCount; i++) {
        ans.push_back(m_nodes[i].aabb);
    }
    return ans;
}


inline bool DynamicTree::Query(std::vector<struct Polygon>& ans,  struct Polygon& polygon) 
{
    //使用栈来遍历二叉树
    std::stack<int> s;
    s.push(m_root);
    AABB aabb = polygon.GetAABB();
  
    while (!s.empty())
    {
        int nodeId = s.top();
        s.pop();
        if (nodeId == -1)
        {
            continue;
        }
        const TreeNode* node = m_nodes + nodeId;
        //与当前节点的AABB相交才进行遍历查找
        if (TestOverlap(node->aabb, aabb))
        {
            //叶子节点进行相交操作
            if (node->IsLeaf())
            {
                if (node->userData != nullptr && node->userData != &polygon) {
                    std::vector<struct Polygon>temp;
                    if (IntersectionPolygons(*node->userData, polygon, temp)) {
                        ans.insert(ans.end(), temp.begin(), temp.end());
                    }
                }
            }
            else
            {
                //遍历左右子树
                s.push(node->child1);
                s.push(node->child2);
            }
        }
    }
    return ans.size() != 0;
}

