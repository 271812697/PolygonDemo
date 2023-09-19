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
    struct Polygon* userData;

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
    /// Constructing the tree initializes the node pool.
    DynamicTree();

    /// Destroy the tree, freeing the node pool.
    ~DynamicTree();

    /// Create a proxy. Provide a tight fitting AABB and a userData pointer.
    int CreateProxy(const AABB& aabb, struct Polygon* userData);

    /// Destroy a proxy. This asserts if the id is invalid.
    void DestroyProxy(int proxyId);


    /// Get proxy user data.
    /// @return the proxy user data or 0 if the id is invalid.
    void* GetUserData(int proxyId) const;


    /// Get the fat AABB for a proxy.
    const AABB& GetFatAABB(int proxyId) const;

    std::vector<AABB> GetAllAABB();

    /// Query an AABB for overlapping proxies. The callback class
    /// is called for each proxy that overlaps the supplied AABB.
    
    bool Query(std::vector<struct Polygon>& ans,  struct Polygon& aabb) ;

 
    int GetHeight() const;

    /// Get the maximum balance of an node in the tree. The balance is the difference
    /// in height of the two children of a node.
    int GetMaxBalance() const;

    /// Get the ratio of the sum of the node areas to the root area.
    float GetAreaRatio() const;

    /// Build an optimal tree. Very expensive. For testing.
    void RebuildBottomUp();



private:

    int AllocateNode();
    void FreeNode(int node);

    void InsertLeaf(int node);
    void RemoveLeaf(int node);

    int Balance(int index);

    int ComputeHeight() const;
    int ComputeHeight(int nodeId) const;


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
        if (TestOverlap(node->aabb, aabb))
        {
            if (node->IsLeaf())
            {
                if (node->userData != nullptr && node->userData != &polygon) {
                    std::vector<struct Polygon>temp;
                    if (IntersectionPolygons(*node->userData, polygon, temp)) {
                        ans.insert(ans.begin(),temp.begin(),temp.end());
                    }
                }
            }
            else
            {
                s.push(node->child1);
                s.push(node->child2);
            }
        }
    }
    return ans.size() != 0;
}

