#include"Dynamic_Tree.h"
#include <string.h>

DynamicTree::DynamicTree()
{
    m_root = -1;

    m_nodeCapacity = 16;
    m_nodeCount = 0;
    m_nodes = (TreeNode*)malloc(m_nodeCapacity * sizeof(TreeNode));
    memset(m_nodes, 0, m_nodeCapacity * sizeof(TreeNode));
    //为空闲节点建立链表，
    for (int i = 0; i < m_nodeCapacity - 1; ++i)
    {
        m_nodes[i].next = i + 1;
        m_nodes[i].height = -1;
    }
    m_nodes[m_nodeCapacity - 1].next = -1;
    m_nodes[m_nodeCapacity - 1].height = -1;
    m_freeList = 0;

    m_insertionCount = 0;
}

DynamicTree::~DynamicTree()
{

    free(m_nodes);
}

//从m_nodes数组中freelist链表分配一个节点（返回其索引）
int DynamicTree::AllocateNode()
{
    // Expand the node pool as needed.
    if (m_freeList == -1)
    {
        assert(m_nodeCount == m_nodeCapacity);


        //数组中的可用空闲节点为空，m_nodes扩容一倍
        TreeNode* oldNodes = m_nodes;
        m_nodeCapacity *= 2;
        m_nodes = (TreeNode*)malloc(m_nodeCapacity * sizeof(TreeNode));
        memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(TreeNode));
        free(oldNodes);


        for (int i = m_nodeCount; i < m_nodeCapacity - 1; ++i)
        {
            m_nodes[i].next = i + 1;
            m_nodes[i].height = -1;
        }
        m_nodes[m_nodeCapacity - 1].next = -1;
        m_nodes[m_nodeCapacity - 1].height = -1;
        m_freeList = m_nodeCount;
    }


    int nodeId = m_freeList;
    m_freeList = m_nodes[nodeId].next;
    m_nodes[nodeId].parent = -1;
    m_nodes[nodeId].child1 = -1;
    m_nodes[nodeId].child2 = -1;
    m_nodes[nodeId].height = 0;
    m_nodes[nodeId].userData = nullptr;
    ++m_nodeCount;
    return nodeId;
}


//将弃用节点返回至m_nodes（链接到freelist）
void DynamicTree::FreeNode(int nodeId)
{
    assert(0 <= nodeId && nodeId < m_nodeCapacity);
    assert(0 < m_nodeCount);
    m_nodes[nodeId].next = m_freeList;
    m_nodes[nodeId].height = -1;
    m_freeList = nodeId;
    --m_nodeCount;
}

//从节点池中分配一个空闲节点，插入（给其索引id）到树中成为叶子节点
int DynamicTree::CreateProxy(const AABB& aabb, struct Polygon* userData)
{
    int proxyId = AllocateNode();


    Vec2 r(0.1, 0.1);
    m_nodes[proxyId].aabb.lowerBound = aabb.lowerBound - r;
    m_nodes[proxyId].aabb.upperBound = aabb.upperBound + r;
    m_nodes[proxyId].userData = userData;
    m_nodes[proxyId].height = 0;
    InsertLeaf(proxyId);

    return proxyId;
}
//将代理叶节点从树中移除，并返回给节点池
void DynamicTree::DestroyProxy(int proxyId)
{
    assert(0 <= proxyId && proxyId < m_nodeCapacity);
    assert(m_nodes[proxyId].IsLeaf());

    RemoveLeaf(proxyId);
    FreeNode(proxyId);
}

//将一个节点插入到树中
void DynamicTree::InsertLeaf(int leaf)
{
    ++m_insertionCount;
    //根节点为空，设置为根节点并返回
    if (m_root == -1)
    {
        m_root = leaf;
        m_nodes[m_root].parent = -1;
        return;
    }

    //为插入叶节点找到一个最佳的index,也就是兄弟节点(花费最小的节点)
    AABB leafAABB = m_nodes[leaf].aabb;
    int index = m_root;
    while (m_nodes[index].IsLeaf() == false)
    {
        int child1 = m_nodes[index].child1;
        int child2 = m_nodes[index].child2;

        float area = m_nodes[index].aabb.GetPerimeter();

        AABB combinedAABB;
        combinedAABB.Combine(m_nodes[index].aabb, leafAABB);
        float combinedArea = combinedAABB.GetPerimeter();

        float cost = 2.0f * combinedArea;

        float inheritanceCost = 2.0f * (combinedArea - area);

        //与child1成为兄弟节点的花费
        float cost1;
        //根据child1是否是叶子节点，区别处理
        if (m_nodes[child1].IsLeaf())
        {
            AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child1].aabb);
            cost1 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child1].aabb);
            float oldArea = m_nodes[child1].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();
            cost1 = (newArea - oldArea) + inheritanceCost;
        }

        //与child2成为兄弟节点的花费
        float cost2;
        if (m_nodes[child2].IsLeaf())
        {
            AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child2].aabb);
            cost2 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child2].aabb);
            float oldArea = m_nodes[child2].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();
            cost2 = newArea - oldArea + inheritanceCost;
        }

        if (cost < cost1 && cost < cost2)
        {
            break;
        }

        if (cost1 < cost2)
        {
            index = child1;
        }
        else
        {
            index = child2;
        }
    }

    int sibling = index;
    int oldParent = m_nodes[sibling].parent;
    int newParent = AllocateNode();
    m_nodes[newParent].parent = oldParent;
    m_nodes[newParent].userData = nullptr;
    m_nodes[newParent].aabb.Combine(leafAABB, m_nodes[sibling].aabb);
    m_nodes[newParent].height = m_nodes[sibling].height + 1;

    if (oldParent != -1)
    {

        if (m_nodes[oldParent].child1 == sibling)
        {
            m_nodes[oldParent].child1 = newParent;
        }
        else
        {
            m_nodes[oldParent].child2 = newParent;
        }

        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leaf;
        m_nodes[sibling].parent = newParent;
        m_nodes[leaf].parent = newParent;
    }
    else
    {
        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leaf;
        m_nodes[sibling].parent = newParent;
        m_nodes[leaf].parent = newParent;
        m_root = newParent;
    }

    //逆向调整树高和AABB
    index = m_nodes[leaf].parent;
    while (index != -1)
    {
        index = Balance(index);

        int child1 = m_nodes[index].child1;
        int child2 = m_nodes[index].child2;

        assert(child1 != -1);
        assert(child2 != -1);

        m_nodes[index].height = 1 + Max(m_nodes[child1].height, m_nodes[child2].height);
        m_nodes[index].aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

        index = m_nodes[index].parent;
    }

}

void DynamicTree::RemoveLeaf(int leaf)
{
    if (leaf == m_root)
    {
        m_root = -1;
        return;
    }

    int parent = m_nodes[leaf].parent;
    int grandParent = m_nodes[parent].parent;
    int sibling;
    //找到兄弟节点
    if (m_nodes[parent].child1 == leaf)
    {
        sibling = m_nodes[parent].child2;
    }
    else
    {
        sibling = m_nodes[parent].child1;
    }

    if (grandParent != -1)
    {
        // Destroy parent and connect sibling to grandParent.
        if (m_nodes[grandParent].child1 == parent)
        {
            m_nodes[grandParent].child1 = sibling;
        }
        else
        {
            m_nodes[grandParent].child2 = sibling;
        }
        m_nodes[sibling].parent = grandParent;
        FreeNode(parent);


        int index = grandParent;
        while (index != -1)
        {
            index = Balance(index);

            int child1 = m_nodes[index].child1;
            int child2 = m_nodes[index].child2;

            m_nodes[index].aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);
            m_nodes[index].height = 1 + Max(m_nodes[child1].height, m_nodes[child2].height);

            index = m_nodes[index].parent;
        }
    }
    else
    {
        m_root = sibling;
        m_nodes[sibling].parent = -1;
        FreeNode(parent);
    }
}

//平衡树的高度
int DynamicTree::Balance(int iA)
{
    assert(iA != -1);

    TreeNode* A = m_nodes + iA;
    if (A->IsLeaf() || A->height < 2)
    {
        return iA;
    }

    int iB = A->child1;
    int iC = A->child2;
    assert(0 <= iB && iB < m_nodeCapacity);
    assert(0 <= iC && iC < m_nodeCapacity);

    TreeNode* B = m_nodes + iB;
    TreeNode* C = m_nodes + iC;

    int balance = C->height - B->height;

    // Rotate C up
    if (balance > 1)
    {
        int iF = C->child1;
        int iG = C->child2;
        TreeNode* F = m_nodes + iF;
        TreeNode* G = m_nodes + iG;
        assert(0 <= iF && iF < m_nodeCapacity);
        assert(0 <= iG && iG < m_nodeCapacity);

        // Swap A and C
        C->child1 = iA;
        C->parent = A->parent;
        A->parent = iC;

        // A's old parent should point to C
        if (C->parent != -1)
        {
            if (m_nodes[C->parent].child1 == iA)
            {
                m_nodes[C->parent].child1 = iC;
            }
            else
            {
                assert(m_nodes[C->parent].child2 == iA);
                m_nodes[C->parent].child2 = iC;
            }
        }
        else
        {
            m_root = iC;
        }

        // Rotate
        if (F->height > G->height)
        {
            C->child2 = iF;
            A->child2 = iG;
            G->parent = iA;
            A->aabb.Combine(B->aabb, G->aabb);
            C->aabb.Combine(A->aabb, F->aabb);

            A->height = 1 + Max(B->height, G->height);
            C->height = 1 + Max(A->height, F->height);
        }
        else
        {
            C->child2 = iG;
            A->child2 = iF;
            F->parent = iA;
            A->aabb.Combine(B->aabb, F->aabb);
            C->aabb.Combine(A->aabb, G->aabb);

            A->height = 1 + Max(B->height, F->height);
            C->height = 1 + Max(A->height, G->height);
        }

        return iC;
    }

    // Rotate B up
    if (balance < -1)
    {
        int iD = B->child1;
        int iE = B->child2;
        TreeNode* D = m_nodes + iD;
        TreeNode* E = m_nodes + iE;
        assert(0 <= iD && iD < m_nodeCapacity);
        assert(0 <= iE && iE < m_nodeCapacity);

        // Swap A and B
        B->child1 = iA;
        B->parent = A->parent;
        A->parent = iB;

        // A's old parent should point to B
        if (B->parent != -1)
        {
            if (m_nodes[B->parent].child1 == iA)
            {
                m_nodes[B->parent].child1 = iB;
            }
            else
            {
                assert(m_nodes[B->parent].child2 == iA);
                m_nodes[B->parent].child2 = iB;
            }
        }
        else
        {
            m_root = iB;
        }

        // Rotate
        if (D->height > E->height)
        {
            B->child2 = iD;
            A->child1 = iE;
            E->parent = iA;
            A->aabb.Combine(C->aabb, E->aabb);
            B->aabb.Combine(A->aabb, D->aabb);

            A->height = 1 + Max(C->height, E->height);
            B->height = 1 + Max(A->height, D->height);
        }
        else
        {
            B->child2 = iE;
            A->child1 = iD;
            D->parent = iA;
            A->aabb.Combine(C->aabb, D->aabb);
            B->aabb.Combine(A->aabb, E->aabb);

            A->height = 1 + Max(C->height, D->height);
            B->height = 1 + Max(A->height, E->height);
        }

        return iB;
    }

    return iA;
}









