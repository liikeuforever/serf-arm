#include <gtest/gtest.h>

#include "serf/utils/Array.h"

TEST(TestArray, TestCon) {
    Array<int> arr(10);
    EXPECT_TRUE(arr.length == 10);
}

TEST(TestArray, TestInitList) {
    Array<int> arr = {1, 2, 3};
    EXPECT_TRUE(arr.length == 3);
}

TEST(TestArray, TestCopy) {
    Array<int> arr = {1, 2, 3};
    Array<int> newArr = arr;
    EXPECT_TRUE(newArr.length == 3);
    EXPECT_EQ(newArr[0], 1);
    EXPECT_EQ(newArr[1], 2);
    EXPECT_EQ(newArr[2], 3);
}

TEST(TestArray, TestAssign) {
    Array<int> arr = {1, 2, 3};
    Array<int> newArr = {4, 5, 6, 7};
    newArr = arr;
    EXPECT_TRUE(newArr.length == 3);
    EXPECT_EQ(newArr[0], 1);
    EXPECT_EQ(newArr[1], 2);
    EXPECT_EQ(newArr[2], 3);
}

TEST(TestArray, TestAccess) {
    Array<int> arr = {1, 2, 3};
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(TestArray, TestModify) {
    Array<int> arr = {1, 2, 3};
    arr[0] = 4; arr[1] = 5; arr[2] = 6;
    EXPECT_EQ(arr[0], 4);
    EXPECT_EQ(arr[1], 5);
    EXPECT_EQ(arr[2], 6);
}