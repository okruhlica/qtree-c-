#include "../src/qtree.h"
#include "gtest/gtest.h"

TEST(ZeroTest, ZeroTest) {
    EXPECT_EQ(1, 0+1);
}

TEST(Trivial, TrivialInsert){
    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    tree.insert({0.5,0.5});
    auto found_pts = tree.points_in_rect({0,0,1,1});
    EXPECT_EQ(1,(*found_pts).size());
}

TEST(Trivial, TrivialInsertOutOfBounds){
    AdamLib::QuadTree tree = AdamLib::QuadTree({0,0,100,100});
    EXPECT_ANY_THROW(tree.insert({-0.5,-0.5}););
}