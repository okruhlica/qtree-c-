#include <iostream>
#include <vector>
#include <set>
#include "qtree.h"
using namespace std;
using namespace AdamLib;

float random_float(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

int main() {
    AABBRect aabb {
            .x0 = 0,
            .x1 = 100,
            .y0 = 0,
            .y1 = 100
    };
    AABBRect query = AABBRect({0,0,10,10});
    QuadTree tree = QuadTree(aabb);

    auto naive = new set<pair<float, float>>;
    int i = 1000* 1000;
    while(i--){
        float x = random_float(0, 100);
        float y = random_float(0, 100);
        tree.insert(XY(x,y));
         if(query.contains(x,y)){
             naive->insert({x,y});
         }
    }

    auto results = tree.points_in_rect(query);
    cout << "Results: Has the following " << results->size() << " elements:" << endl;
    for(auto res: *results){
        // cout << res.x << " " << res.y << endl;
    }

    cout << "Check: Should have the following " << naive->size() << " elements:" << endl;
    for(auto item: *naive){
       // cout << item.first << " " << item.second << endl;
    }

    delete naive;
    return 0;
}
