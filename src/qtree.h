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
        inline bool operator==(const XY& rhs){
            return rhs.x == x && rhs.y == y;
        }
    };

    struct AABBRect {
        float x0, y0, x1, y1;
        AABBRect() = default;

        bool contains(float x, float y) const;

        bool contains(XY pt) const;

        AABBRect intersection(AABBRect rect) const;

        bool is_empty() const;

        bool covers(AABBRect* other) const;

    };


    struct QuadNode
    {
        AABBRect aabb;
        // Points to the first child if this node is a branch or the first
        // element if this node is a leaf.
        int32_t child_nw;
        int32_t values;
        // Stores the number of elements in the leaf or -1 if it this node is
        // not a leaf.
        int8_t count;

        explicit QuadNode(AABBRect aabb, int32_t values);
        inline bool is_leaf() const;

        inline bool is_internal() const;

        inline bool is_full_leaf() const;

        inline void set_leaf(int32_t vindex);

        inline void set_internal();

        };

    class QuadTree {
    public:
        explicit QuadTree(AABBRect aabb);

        void insert(float value_x, float value_y);
        void insert(XY pointXY);
        bool contains(XY pointXY);
        void remove(XY pointXY);
        std::vector<XY> *points_in_rect(AABBRect rect) const;
        std::vector<XY> *points_in_circle(XY center, float radius) const;
        void debug_inspect(int node=0,int depth=0) const;
        ~QuadTree();

    private:
        const int NODE_CAPACITY = 8;
        int32_t last_point_id = 8;
        std::vector<QuadNode*> * nodes = new std::vector<QuadNode*>();
        std::vector<XY>* points;

        QuadNode* root() const;
        void _subdivide(QuadNode* node);

        static inline short find_quadrant(float x, float y, float x0, float y0, float x1, float y1);
        static inline short find_quadrant(XY xy, AABBRect rect);
        inline void add_value(QuadNode* node, XY val);
        inline QuadNode* node_for_value(XY val) const;
        inline int32_t index_for_value(const QuadNode* node, XY val) const;

    };
}
#endif //QUADTREE_QTREE_H
