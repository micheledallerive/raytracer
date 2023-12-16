//
// Created by michele on 18.11.23.
//

#include "tracers/kdtree.h"
#include "objects/plane.h"
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>


std::optional<Hit> KDTreeTracer::traverse(const Ray &ray,
                                          const size_t currentNodeIdx,
                                          const float tmin,
                                          const float tmax) const
{
    if (currentNodeIdx >= nodes.size()) {
        throw std::runtime_error("Node index out of bounds");
    }

    const auto &node = nodes[currentNodeIdx];
    if (node.isLeaf()) {
        if (node.getNumObjects() == 1) {
            const auto &object = objects[node.getSingleObject()];
            if (!object->getBoundingBox().intersect(ray)) {
                return std::nullopt;
            }
            return object->intersect(ray);
        }

        std::optional<Hit> closestHit;
        for (const auto &objectIndex : leafObjectIndices[node.getObjectsOffset()]) {
            const auto &object = objects[objectIndex];
            if (!object->getBoundingBox().intersect(ray)) {
                continue;
            }
            auto hit = object->intersect(ray);
            if (hit && (!closestHit || hit->distance < closestHit->distance)) {
                closestHit = hit;
            }
        }
        return closestHit;
    }

    const int axis = node.getAxis();
    const float split = node.getSplit();

    const float tsplit = (split - ray.origin[axis]) / ray.direction[axis];
    int frontChild = currentNodeIdx + 1;
    int backChild = node.getChild();

    if (ray.direction[axis] < 0 || (ray.direction[axis] == 0 && ray.origin[axis] <= split)) {
        std::swap(frontChild, backChild);
    }

    if (tsplit < tmin) {
        return traverse(ray, backChild, tmin, tmax);
    } else if (tsplit > tmax) {
        return traverse(ray, frontChild, tmin, tmax);
    } else {
        const auto frontHit = traverse(ray, frontChild, tmin, tsplit);
        if (frontHit && frontHit->distance < tsplit) {
            return frontHit;
        }
        return traverse(ray, backChild, tsplit, tmax);
    }
}

std::optional<Hit> KDTreeTracer::trace(const Ray &ray) const
{
    return traverse(ray, 0, 0, std::numeric_limits<float>::max());
}


KDTreeTracer::KDTreeTracer(std::vector<std::shared_ptr<Object>> &_objects)
    : Tracer(_objects)
{
    std::vector<int> idxs(objects.size());
    std::iota(idxs.begin(), idxs.end(), 0);

    const auto startTime = clock();
    construct(0, 0, std::move(idxs));
    const auto endTime = clock();
    std::cout << "KDTree construction time: " << (endTime - startTime) / (double) CLOCKS_PER_SEC << "s" << std::endl;
}

void KDTreeTracer::construct(const int nodeIndex, const int depth, std::vector<int> obj_indices)
{
    if ((size_t) nodeIndex >= nodes.size()) {
        nodes.resize(nodes.size() + std::min((size_t) 512, (size_t) log2((double) objects.size())));
    }

    auto &node = nodes[nodeIndex];

    if (obj_indices.size() <= KDTreeTracer::MAX_OBJECTS_PER_NODE || depth > 1 * log2((double) objects.size())) {
        node.setupLeafNode(obj_indices, leafObjectIndices);
        return;
    }

    const int axis = (depth + 1) % 3;

    std::nth_element(obj_indices.begin(),
                     obj_indices.begin() + obj_indices.size() / 2 + 1,
                     obj_indices.end(),
                     [axis, this](const int a, const int b)
                     {
                         const auto &boxA = objects[a]->getBoundingBox();
                         const auto &boxB = objects[b]->getBoundingBox();
                         return boxA.getCenter()[axis] < boxB.getCenter()[axis];
                     });

    const float split = objects[obj_indices[obj_indices.size() / 2 + 1]]->getBoundingBox().getCenter()[axis];
    const auto leftIter = std::partition(obj_indices.begin(), obj_indices.end(), [axis, split, this](const int a)
    {
        const auto &box = objects[a]->getBoundingBox();
        return box.max[axis] < split;
    });

    const auto overlappingIter = std::partition(leftIter, obj_indices.end(), [axis, split, this](const int a)
    {
        const auto &box = objects[a]->getBoundingBox();
        return box.min[axis] <= split;
    });

    std::vector<int> leftIndices;
    std::vector<int> rightIndices;
    std::move(obj_indices.begin(), leftIter, std::back_inserter(leftIndices));
    std::copy(leftIter, overlappingIter, std::back_inserter(leftIndices));
    std::move(leftIter, obj_indices.end(), std::back_inserter(rightIndices));

    if (leftIndices.size() == obj_indices.size() || rightIndices.size() == obj_indices.size()) {
        node.setupLeafNode(obj_indices, leafObjectIndices);
        return;
    }

    const int leftIndex = ++nextFreeNode;

    construct(leftIndex, depth + 1, std::move(leftIndices));

    const int rightIndex = ++nextFreeNode;
    nodes[nodeIndex].setupInnerNode(axis, split, rightIndex);

    construct(rightIndex, depth + 1, std::move(rightIndices));
}