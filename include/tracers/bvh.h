#ifndef DEBUG_BUILD
#define DEBUG_BUILD 0
#endif

#define LOG(x)                  \
    if (DEBUG_BUILD) {          \
        std::cout << x << '\n'; \
    }

#define PRINT(x)                              \
    if (DEBUG_BUILD) {                        \
        std::cout << #x << ": " << x << '\n'; \
    }

#pragma once

#include "../objects/box.h"
#include "../objects/triangle.h"
#include "tracer.h"
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

using glm::vec3;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::optional;
using std::pair;
using std::vector;

class BVHNode
{
public:
    Box bounds;
    vector<Triangle> triangles;
    BVHNode *left;
    BVHNode *right;

    explicit BVHNode(vector<Triangle> &arg_triangles, int axis = 0)
    {
        triangles = vector<Triangle>(arg_triangles);
        bounds = calculateBoundingBox(triangles);

        if (triangles.size() <= 20) {
            this->left = nullptr;
            this->right = nullptr;
            this->triangles = vector<Triangle>(arg_triangles);
        }
        else {
            pair<vector<Triangle>, vector<Triangle>> split = splitTrianglesSpace(triangles, axis);
            vector<Triangle> &l = split.first;
            vector<Triangle> &r = split.second;

            this->left = new BVHNode(l, (axis + 1) % 3);
            this->right = new BVHNode(r, (axis + 1) % 3);
        }
    }

    static Box calculateBoundingBox(vector<Triangle> &arg_triangles)
    {
        Box box;

        for (Triangle &triangle : arg_triangles) {
            box.merge(triangle.getBoundingBox());
        }
        return box;
    }

    static pair<vector<Triangle>, vector<Triangle>>
    splitTrianglesSpace(vector<Triangle> &arg_triangles, int axis)
    {
        vector<Triangle> l;
        vector<Triangle> r;

        float mid = 0;
        for (const Triangle &triangle : arg_triangles) {
            mid += triangle.points[0][axis] + triangle.points[1][axis] + triangle.points[2][axis];
        }

        mid /= arg_triangles.size() * 3;

        for (const Triangle &triangle : arg_triangles) {
            if (triangle.points[0][axis] < mid || triangle.points[1][axis] < mid || triangle.points[2][axis] < mid) {
                l.emplace_back(triangle);
            }
            else {
                r.emplace_back(triangle);
            }
        }

        return {l, r};
    }

    vector<Triangle> intersect(const Ray &ray) const
    {

        if (this->left == nullptr && this->right == nullptr) {
            return this->triangles;
        }

        // If right and left, return both
        auto opt_l = optional<Hit>(std::nullopt);
        auto opt_r = optional<Hit>(std::nullopt);
        if (this->left != nullptr) {
            opt_l = this->left->bounds.intersect(ray);
        }
        if (this->right != nullptr) {
             opt_r = this->right->bounds.intersect(ray);
        }

        if (opt_l.has_value() && opt_r.has_value()) {
            vector<Triangle> leftpart = this->left->intersect(ray);
            vector<Triangle> rightpart = this->right->intersect(ray);
            leftpart.reserve(leftpart.size() + rightpart.size());
            for (auto &t : rightpart) leftpart.push_back(std::move(t));
            return leftpart;
        }

        // if left, return left
        if (opt_l.has_value()) {
            return this->left->intersect(ray);
        }

        // if right, return right
        if (opt_r.has_value()) {
            return this->right->intersect(ray);
        }

        return this->triangles;
    }
};

class BVHTracer: public Tracer
{
public:
    BVHNode *root;

    optional<Hit> trace(const Ray &ray) const override
    {
        optional<Hit> closest_hit;
        optional<Hit> hit = root->bounds.intersect(ray);

        LOG("trying")
        PRINT(hit.has_value())
        if (!hit.has_value()) {
            return std::nullopt;
        }

        for (Triangle &t : this->root->intersect(ray)) {
            optional<Hit> opt_h = t.intersect(ray);

            if (opt_h && (!closest_hit || opt_h->distance < closest_hit->distance)) {
                closest_hit = opt_h;
            }
        }

        LOG("hit")
        return closest_hit;
    }

    explicit BVHTracer(vector<std::unique_ptr<Object>> &_objects) : Tracer(_objects)
    {
        vector<int> idxs(objects.size());
        std::iota(idxs.begin(), idxs.end(), 0);

        const auto startTime = clock();
        //construct(0, 0, std::move(idxs));
        vector<Triangle> triangles;
        triangles.reserve(objects.size());
        for (auto &ptr : objects) {
            triangles.push_back(*dynamic_cast<Triangle *>(ptr.get()));
        }
        this->root = new BVHNode(triangles);
        const auto endTime = clock();
        cout << "BVH construction time: " << (endTime - startTime) / (double) CLOCKS_PER_SEC << "s" << endl;
    }

    explicit BVHTracer(vector<Triangle> &triangles)
    {
        this->root = new BVHNode(triangles);
    }
};