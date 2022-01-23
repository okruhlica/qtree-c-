//
// Created by Adam Okruhlica on 23/01/2022.
//

#ifndef QUADTREE_QTREE_H
#define QUADTREE_QTREE_H
#include <vector>
namespace AdamLib {
    struct XY {
        float x, y;
        XY(float x, float y);
        XY();
    };

    struct AABBRect {
        float x0, y0, x1, y1;
        AABBRect() = default;

        bool contains(float x, float y) const;

        bool contains(XY pt) const;

        AABBRect intersection(AABBRect rect) const;

        bool is_empty() const;
    };


    struct QuadNode
    {
        // Points to the first child if this node is a branch or the first
        // element if this node is a leaf.
        int32_t child_nw;

        // Stores the number of elements in the leaf or -1 if it this node is
        // not a leaf.
        int8_t count;
        int32_t values;
        AABBRect aabb{};

        explicit QuadNode(AABBRect aabb, int32_t child_nw = 0, int8_t count = -1, int32_t values = -1);
        inline bool is_leaf() const;

        inline bool is_internal() const;

        inline void set_leaf(int32_t vindex);

        inline void set_internal();
    };


    class QuadTree {
    public:
        explicit QuadTree(AABBRect aabb);

        void insert(float value_x, float value_y);

        void insert(XY pt);

        std::vector<XY> *points_in_rect(AABBRect rect);
        ~QuadTree();

    private:
        const int NODE_CAPACITY = 8;
        AABBRect bbox{};
        std::vector<QuadNode*> * nodes = new std::vector<QuadNode*>();
        std::vector<XY>* points = new std::vector<XY>();

        QuadNode* root() const;

        void extend_nodes(int n, XY value);

        bool add_to_leaf(QuadNode * node, XY xy);
        inline short find_quadrant(float x, float y, float x0, float y0, float x1, float y1);
        inline short find_quadrant(XY xy, AABBRect rect);

    };
}
#endif //QUADTREE_QTREE_H
