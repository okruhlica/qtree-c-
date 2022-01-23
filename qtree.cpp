//
// Created by Adam Okruhlica on 23/01/2022.
//

#include "qtree.h"
#include <vector>

using namespace AdamLib;
using namespace std;

XY::XY() = default;

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

    if ((cx1 > cx0) && (cy1 > cy0)) {
        return AABBRect({cx0, cy0, cx1, cy1});
    } else return {};
}

bool AABBRect::is_empty() const {
    return (this->x0 >= this->x1) || (this->y0 >= this->y1);
}


QuadNode::QuadNode(AABBRect aabb, int32_t child_nw, int8_t count, int32_t values) {
    this->child_nw = child_nw;
    this->count = count;
    this->values = values;
    this->aabb = aabb;
}

inline bool QuadNode::is_leaf() const {
    return (values >= 0) & (child_nw == 0);
}

inline bool QuadNode::is_internal() const {
    return child_nw > 0;
}

inline void QuadNode::set_leaf(int32_t vindex) {
    child_nw = 0;
    values = vindex;
}

inline void QuadNode::set_internal() {
    values = -1;
    count = -1;
}

inline float mid(float a, float b) {
    return a + (b - a) / 2;
}


QuadTree::QuadTree(AABBRect aabb) {
    bbox = aabb;
    nodes->push_back(new QuadNode(bbox, 0, -1, 0));
    points->resize(NODE_CAPACITY + 1); // todo
}


void QuadTree::insert(float value_x, float value_y) {
    this->insert(XY(value_x, value_y));
}

void QuadTree::insert(XY pt) {
    if (!bbox.contains(pt)) throw invalid_argument("Point is out of bounds!");

    auto *process_queue = new vector<pair<int32_t, XY>>();
    pair<int32_t, XY> entry{0, pt};
    process_queue->push_back(entry);


    // Otherwise initialize the process_queue queue with the root node.
    // In each step a node + value is taken from the queue and the value is tried
    // input into the node. This step may yield new nodes to process_queue (if a node is being split).
    while (!process_queue->empty()) {
        pair<int32_t, XY> item_to_process = process_queue->back();
        process_queue->pop_back();

        auto node = (*nodes)[item_to_process.first];
        XY point{item_to_process.second};
        AABBRect aabb = node->aabb;

        if (node->is_internal()) { // Need to traverse deeper to insert this value.
            auto child_quad_node = node->child_nw + find_quadrant(point, aabb);
            process_queue->push_back(pair<int32_t, XY>{child_quad_node, point});
            continue;
        }

        if (node->is_leaf()) {
            if (this->add_to_leaf(node, point)) {
                continue;
            }

            // We need to subdivide the node
            int32_t children_count = nodes->size();
            node->child_nw = children_count;

            // Construct the child nodes
            float mx = mid(aabb.x0, aabb.x1);
            float my = mid(aabb.y0, aabb.y1);

            AABBRect nw_child = AABBRect({aabb.x0, aabb.y0, mx, my});
            AABBRect ne_child = AABBRect({mx, aabb.y0, aabb.x1, my});
            AABBRect se_child = AABBRect({mx, my, aabb.x1, aabb.y1});
            AABBRect sw_child = AABBRect({aabb.x0, my, mx, aabb.y1});

            int32_t points_index = this->points->size();
            nodes->push_back(new QuadNode(nw_child, 0, -1, points_index));
            nodes->push_back(new QuadNode(ne_child, 0, -1, points_index + NODE_CAPACITY));
            nodes->push_back(new QuadNode(se_child, 0, -1, points_index + 2 * NODE_CAPACITY));
            nodes->push_back(new QuadNode(sw_child, 0, -1, points_index + 3 * NODE_CAPACITY));

            extend_nodes(4 * NODE_CAPACITY, {0, 0}); // TODO: How to do this prop?

            // Place the new point into proper quadrant
            int quadrant_new = find_quadrant(point, aabb);
            int node_for_new = node->child_nw + quadrant_new;
            process_queue->push_back({node_for_new, point});

            // Place old values into proper quadrants
            for (int i = node->values; i < (node->values + node->count + 1); i++) {
                auto pt = (*points)[i];
                int quadrant_old = find_quadrant(pt, aabb);
                int node_for_old = node->child_nw + quadrant_old;
                process_queue->push_back({node_for_old, pt});
            }
            node->set_internal();
        }
    }
}

QuadNode *QuadTree::root() const {
    return (*nodes)[0];
}

vector<XY> *QuadTree::points_in_rect(AABBRect rect) {
    auto points_found = new vector<XY>{};

    // Trivial case; the search area does not cross our bounding box. Returns empty result set.
    AABBRect working_rect = rect.intersection(root()->aabb);
    if (working_rect.is_empty()) {
        return points_found;
    }

    auto process_queue = new vector<pair<int32_t, AABBRect>>;
    process_queue->push_back({0, working_rect});

    while (!process_queue->empty()) {
        auto item = process_queue->back();
        process_queue->pop_back();

        auto node_idx = item.first;
        auto node_aabb = item.second;
        QuadNode *node = (*nodes)[node_idx];

        auto intersection = node->aabb.intersection(node_aabb);
        if (intersection.is_empty()) continue;

        if (node->is_leaf()) {
            for (int i = node->values; i < node->values + node->count + 1; i++) {
                if (node_aabb.contains((*points)[i])) {
                    points_found->push_back((*points)[i]);
                }
            }
        }

        if (node->is_internal()) {
            for (int i = 0; i < 4; i++) {
                process_queue->push_back({node->child_nw + i, node_aabb});
            }
        }
    }

    delete process_queue; // TODO: why heap?
    return points_found;
}

QuadTree::~QuadTree() {
    delete nodes;
    delete points;
}

void QuadTree::extend_nodes(int n, XY value) {
    while (n--) {
        points->push_back(value);
    }
}

bool QuadTree::add_to_leaf(QuadNode *node, XY xy) {
    if (node->count >= NODE_CAPACITY - 1) {
        return false;
    }

    ++node->count;
    (*points)[node->values + node->count] = xy;
    return true;
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
