#include "../src/qtree.h"
#include "gtest/gtest.h"


TEST(Trivial, TrivialInsert){
    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    tree.insert({0.5,0.5});
    auto found_pts = tree.points_in_rect({0,0,1,1});
    EXPECT_EQ(1,(*found_pts).size());
}

TEST(Insert, InsertValueOutOfBounds){
    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    EXPECT_ANY_THROW(tree.insert({-0.5,-0.5}));
}

TEST(Insert, InsertValueProper){
    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    EXPECT_NO_THROW(tree.insert({1.0,1.0}));
    EXPECT_NO_THROW(tree.insert({0.0,0.0}));
    EXPECT_NO_THROW(tree.insert({100.0,100.0}));
    EXPECT_NO_THROW(tree.insert({false,false}));
}

TEST(Insert, InsertValueDuplicate){
    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    AdamLib::XY pt = {1.0,1.0};
    EXPECT_NO_THROW(tree.insert(pt));
    EXPECT_NO_THROW(tree.insert(pt));

    auto points = tree.points_in_rect({0,0,1,1});
    EXPECT_EQ(points->size(), 2);

}

TEST(Insert, InsertSet01){
    float test_points[] = {1,1,
    2,2,
    10,0,
    100,100,
    50,47,
    47,50,
    60,3,
    13,24,
    7,7,
    12,80,
    59,99,
    70,87};

    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    for(int i=0;i<23;i+=2) {
        tree.insert({test_points[i], test_points[i+1]});
    }

    EXPECT_EQ(tree.points_in_rect({2,2,2,2})->size(),1);
}

TEST(contains, ContainsTrivial){
    float test_points[] = {1,1,
                           2,2,
                           10,0,
                           100,100,
                           50,47,
                           47,50,
                           60,3,
                           13,24,
                           7,7,
                           12,80,
                           59,99,
                           70,87};

    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    for(int i=0;i<23;i+=2) {
        AdamLib::XY pt = {test_points[i], test_points[i+1]};
        tree.insert(pt);
        EXPECT_TRUE(tree.contains(pt));
    }
    EXPECT_FALSE(tree.contains({0,0}));
    EXPECT_FALSE(tree.contains({-1,-1}));
    EXPECT_FALSE(tree.contains({1.001,1.}));
}


