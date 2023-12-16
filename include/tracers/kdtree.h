//
// Created by michele on 18.11.23.
//

#pragma once


#include "tracer.h"

class KDTreeNode {
private:
    union {
        float split; // inner nodes
        int objectIndicesOffset; // leaf nodes with more than one object
        int singleObject; // leaf nodes with only one object
    }; // mutually exclusive; 4 bytes
    union {
        int axis; // both inner and leaf nodes
        int numObjects; // leaf nodes
        int child; // inner nodes
    }; // almost mutually exclusive; 4 bytes
public:
    static constexpr int LEAF = 3;

    KDTreeNode() = default;

    void setupLeafNode(std::vector<int> &nodeIndices, std::vector<std::vector<int>> &treeObjectIndices)
    {
        axis = LEAF;
        numObjects |= ((int) nodeIndices.size() << 2);
        if (numObjects == 1) {
            singleObject = nodeIndices[0];
        } else {
            objectIndicesOffset = (int) treeObjectIndices.size();
            treeObjectIndices.emplace_back(std::move(nodeIndices));
        }
    }

    void setupInnerNode(int _axis, float _split, int _child)
    {
        axis = _axis;
        split = _split;
        child |= (_child << 2);
    }

    int getAxis() const
    {
        return axis & 3;
    }

    bool isLeaf() const
    {
        return (axis & 3) == LEAF;
    }

    int getObjectsOffset() const
    {
        return objectIndicesOffset;
    }

    float getSplit() const
    {
        return split;
    }

    int getChild() const
    {
        return (child >> 2);
    }

    int getNumObjects() const
    {
        return (numObjects >> 2);
    }

    int getSingleObject() const
    {
        return singleObject;
    }
};

class KDTreeTracer: public Tracer {
public:
    [[nodiscard]] std::optional<Hit> trace(const Ray &ray) const override;

    KDTreeTracer() = default;

    explicit KDTreeTracer(std::vector<std::shared_ptr<Object>> &objects);

    ~KDTreeTracer() override = default;

private:
    static constexpr int MAX_OBJECTS_PER_NODE = 8;
    std::vector<KDTreeNode> nodes;
    int nextFreeNode = 0;
    std::vector<std::vector<int>> leafObjectIndices;

    void construct(int nodeIndex, int depth, std::vector<int> obj_indices);

    [[nodiscard]] std::optional<Hit> traverse(const Ray &ray, size_t currentNodeIdx, float tmin, float tmax) const;
};
