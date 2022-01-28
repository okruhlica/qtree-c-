#include <iostream>
#include <vector>
#include <set>
#include "qtree.h"
#include <chrono>

using namespace std;
using namespace AdamLib;

class Timer{
public:
    Timer(std::string name){
        timer_name = name;
        start_time = std::chrono::high_resolution_clock::now();
    }

    ~Timer(){
        Stop();
    }

    void Stop(){
        auto end_time = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(start_time).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();
        auto duration = end-start;

        cout << "[" << timer_name << "]" << duration*0.001 << "ms" << endl;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::string timer_name;
};

float random_float(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

int main() {
    AABBRect query = AABBRect({0,0,100,10});
    auto tree = QuadTree({0,0,100,100});

    auto naive = new set<pair<float, float>>;
    {
        auto t0 = Timer("Naive populate");
        int i = 1000*1000;
        while(i--){
            float x = random_float(0, 100);
            float y = random_float(0, 100);

             if(query.contains(x,y)){
                 naive->insert({x,y});
             }
             tree.insert({x,y});
        }
    }
//    tree.debug_inspect();

    auto results = tree.points_in_rect(query);
    cout << "Results: Has the following " << results->size() << " elements:" << endl;
    for(auto res: *results){
//         cout << res.x << " " << res.y << endl;
    }

    cout << "Check: Should have the following " << naive->size() << " elements:" << endl;
    for(auto item: *naive){
  //      cout << item.first << " " << item.second << endl;
    }

    auto E = new set<pair<float,float>>;
    for (int i=0;i<results->size();i++){
        E->insert({(*results)[i].x,(*results)[i].y});
    }

    cout << "Missing in solution:\n";
    set<pair<float,float>> diff;
    set_difference(naive->begin(), naive->end(), E->begin(), E->end(), inserter(diff,diff.end()));
    for(auto item: diff){
        cout << item.first << " " << item.second << endl;
    }

    delete naive;
    return 0;
}
