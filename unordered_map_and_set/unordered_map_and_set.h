//
// Created by 遥远 on 2022/11/26.
//

#ifndef CPPCODE_UNORDERED_MAP_AND_SET_H
#define CPPCODE_UNORDERED_MAP_AND_SET_H

#endif //CPPCODE_UNORDERED_MAP_AND_SET_H


//unordered_map_and_set
// map/set 和 unordered_map/unordered_set 的区别:
// 1、都可以实现key和key:value 的搜索场景，并且功能和使用基本一样。
// 2、map/set底层是基于红黑树实现的，他的增删查改的时间复杂度为：O(log N),遍历出来是有序的。
// 3、unordered_map/unordered_set底层是基于哈希表实现的，他的增删查改时间复杂度为:O(1)，遍历出来是无序的。
// 4、map/set 的迭代器是双向迭代器，而unordered_map/unordered_set的迭代器是单向迭代器。

#include <iostream>
using namespace std;


void test_unordered_map_and_set();
void test_set_compare_with_unordered_set();