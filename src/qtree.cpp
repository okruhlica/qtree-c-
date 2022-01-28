//
// Created by Adam Okruhlica on 23/01/2022.
//

#include "qtree.h"
#include <vector>
#include <iostream>

using namespace AdamLib;
using namespace std;


inline float mid(float a, float b) {
    return a + (b - a) / 2;
}

XY::XY(): x(0), y(0){}

XY::XY(float x, float y) {
    this->x = x;
    this->y = y;
}

bool AABBRect::contains(float x, float y) const {
    return (this->x0 <= x) & (this->y0 <= y) & (this->x1 >= x) & (this->y1 >= y);
}

bool AABBRect::contains(XY pt) const {
    return this->contains(pt.x, pt.y);
}

AABBRect AABBRect::intersection(AABBRect rect) const {
    float cx1 = min(this->x1, rect.x1);
    float cx0 = max(this->x0, rect.x0);
    float cy1 = min(this->y1, rect.y1);
    float cy0 = max(this->y0, rect.y0);

    if ((cx1 >= cx0) && (cy1 >= cy0)) {
        return AABBRect({cx0, cy0, cx1, cy1});
    } else return {0, 0, -1, -1};
}

bool AABBRect::is_empty() const {
    return (this->x0 > this->x1) || (this->y0 > this->y1);
}

bool AABBRect::covers(AABBRect *other) const {
    return ((other->x0 >= x0) && (other->x1 <= x1) && (other->y0 >= y0) && (other->y1 <= y1));
}


const int NODE_CAPACITY = 8;

QuadNode::QuadNode(AABBRect aabb, int32_t values) {
    this->child_nw = 0;
    this->count = 0;
    this->values = values;
    this->aabb = aabb;
}

inline bool QuadNode::is_leaf() const {
    return (values > 0) & (child_nw == 0);
}

inline bool QuadNode::is_full_leaf() const {
    return (count >= NODE_CAPACITY) & (child_nw == 0);
}

inline bool QuadNode::is_internal() const {
    return child_nw > 0;
}


QuadTree::QuadTree(AABBRect aabb, int initial_size) {
    bbox = aabb;
    nodes->push_back(new QuadNode(bbox, 0));
    this->points = new vector<XY>(initial_size * 8 + 1);
}

void QuadTree::insert(float value_x, float value_y) {
    this->insert(XY(value_x, value_y));
}

void QuadTree::insert(XY pointXY) {
    if (!bbox.contains(pointXY)) throw invalid_argument("Point is out of bounds!");

    //1. Traverse down the tree and find a leaf to insert to
    auto node = node_for_value(pointXY);

    //2a. We arrived at leaf with remaining capacity; insert the point here.
    if (!node->is_full_leaf()) {
        add_value(node, pointXY);
        return;
    }

    //2b. We arrived at a full leaf; subdivide it until we have space to insert.
    do {
        _subdivide(node);
        node = (*nodes)[node->child_nw + find_quadrant(pointXY, node->aabb)];
    } while (node->is_full_leaf());

    add_value(node, pointXY);
}
inline void QuadTree::add_value(QuadNode* node, XY val){
    (*points)[node->values + node->count] = val;
    ++node->count;
}

void QuadTree::_subdivide(QuadNode *node) {
    node->child_nw = nodes->size();
    auto aabb = node->aabb;

    // Construct the child nodes
    float mx = mid(aabb.x0, aabb.x1);
    float my = mid(aabb.y0, aabb.y1);

    AABBRect nw_child = AABBRect({aabb.x0, aabb.y0, mx, my});
    AABBRect ne_child = AABBRect({mx, aabb.y0, aabb.x1, my});
    AABBRect se_child = AABBRect({mx, my, aabb.x1, aabb.y1});
    AABBRect sw_child = AABBRect({aabb.x0, my, mx, aabb.y1});

    int32_t points_index = this->points->size();
    if(points_index + 4*AdamLib::QuadTree::NODE_CAPACITY > points->capacity())
        points->resize(1.4*points->capacity());

    nodes->push_back(new QuadNode(nw_child, points_index));
    nodes->push_back(new QuadNode(ne_child, points_index + NODE_CAPACITY));
    nodes->push_back(new QuadNode(se_child, points_index + 2 * NODE_CAPACITY));
    nodes->push_back(new QuadNode(sw_child, points_index + 3 * NODE_CAPACITY));

    for (int i = node->values; i < node->values + node->count; ++i) {
        auto new_node = (*nodes)[node->child_nw + find_quadrant((*points)[i], aabb)];
        add_value(new_node, (*points)[i]);
    }
}

std::ostream& operator<<(std::ostream &strm, const AABBRect &a) {
    return strm << "[" << a.x0 << " " << a.y0 << " " << a.x1 << " " << a.y1 << "]";
}
QuadNode *QuadTree::root() const {
    return (*nodes)[0];
}

vector<XY> *QuadTree::points_in_rect(AABBRect rect) {
    auto points_found = new vector<XY>{};

    // Trivial case; the search area does not cross our bounding box. Returns empty result set.
    if (rect.intersection(root()->aabb).is_empty())
        return points_found;

    auto process_queue = new vector<int32_t>;
    process_queue->push_back(0);

    while (!process_queue->empty()) {
        QuadNode *node = (*nodes)[process_queue->back()];
        process_queue->pop_back();

        if(node->aabb.intersection(rect).is_empty())
            continue;

        if (node->is_internal()) {
            for (int i = 0; i < 4; i++)
                process_queue->push_back(node->child_nw + i);
            continue;
        }

        for (int i = node->values; i < node->values + node->count; i++) {
            if (rect.contains((*points)[i])) {
                points_found->push_back((*points)[i]);
            }
        }
    }

    return points_found;
}
inline QuadNode* QuadTree::node_for_value(XY val){
    auto node = (*nodes)[0];
    while (node->is_internal())
        node = (*nodes)[node->child_nw + find_quadrant(val, node->aabb)];
    return node;
}

inline int32_t QuadTree::index_for_value(QuadNode* node, XY val){
    for(int i = node->values; i < node->values + node->count;i++)
        if ((*points)[i] == val) return i;
    return -1;
}

bool QuadTree::contains(XY pointXY){
    auto node = node_for_value(pointXY);
    return index_for_value(node, pointXY) >= 0;
}

bool QuadTree::remove(XY pointXY){
    auto node = node_for_value(pointXY);
    auto index = index_for_value(node, pointXY) >= 0;
    for(int i =index+1; i<node->values + node->count;i++){
        (*points)[i-1] = (*points)[i];
    }
    node->count--;
}

QuadTree::~QuadTree() {
    delete nodes;
    delete points;
}


inline short QuadTree::find_quadrant(float x, float y, float x0, float y0, float x1, float y1) {
    float mx = mid(x0, x1);
    float my = mid(y0, y1);

    if (x < mx) {
        return (y < my) ? 0 : 3;
    }
    return (y < my) ? 1 : 2;
}

inline short QuadTree::find_quadrant(XY xy, AABBRect rect) {
    return find_quadrant(xy.x, xy.y, rect.x0, rect.y0, rect.x1, rect.y1);
}

void QuadTree::debug_inspect() const{
    int n=0, n_internal=0, n_leaves=0, n_full_leaves =0, n_empty_leaves=0;

    for(auto node : (*nodes)){
        n++;
        if (node->is_internal()) n_internal++;
        if (node->is_leaf()) n_leaves++;
        if (node->is_full_leaf()) n_full_leaves++;
        if (node->count == 0) n_empty_leaves++;
    }

    cout << "Nodes:" << n << endl;
    cout << "-- Internal:" << n_internal << endl;
    cout << "-- Leaves:" << n_leaves << endl;
    cout << "---- Full:" << n_full_leaves <<endl;
    cout << "---- Empty:" << n_empty_leaves <<endl;
}