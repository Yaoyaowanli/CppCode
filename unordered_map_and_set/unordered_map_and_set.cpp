//
// Created by 遥远 on 2022/11/26.
//
#include "unordered_map_and_set.h"
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>

void test_unordered_map_and_set(){
    //unordered_set是无序集合，底层使用hashtable实现。
    unordered_set<int> s;
    s.insert(4);
    s.insert(5);
    s.insert(2);
    s.insert(7);
    s.insert(1);
    s.insert(1);
    s.insert(1);
    //遍历
    unordered_set<int>::iterator sit = s.begin();
    while (sit != s.end()){
        cout << *sit << " ";
        sit++;
    }
}

void test_set_compare_with_unordered_set(){
    set<int> s;
    unordered_set<int> us;
    const int n = 1000000;
    vector<int> v;
    v.reserve(n);
    srand(time(0));
    for (size_t i=0; i<n;i++){
        v.push_back(rand());
    }
    //insert
    size_t begin_unordered_set = clock();
    for (size_t i=0; i<n; i++){
        us.insert(v[i]);
    }
    size_t end_unordered_set = clock();
    cout << "unordered_set_insert: " << end_unordered_set-begin_unordered_set << endl;

    size_t begin_set = clock();
    for (size_t i=0; i<n; i++){
        s.insert(v[i]);
    }
    size_t end_set = clock();
    cout << "set_insert: " << end_set-begin_set << endl;

    //find
    size_t begin_unordered_set2 = clock();
    for (size_t i=0; i<n; i++){
        us.find(v[i]);
    }
    size_t end_unordered_set2 = clock();
    cout << "unordered_set_find: " << end_unordered_set2-begin_unordered_set2 << endl;

    size_t begin_set2 = clock();
    for (size_t i=0; i<n; i++){
        s.find(v[i]);
    }
    size_t end_set2 = clock();
    cout << "set_find: " << end_set2-begin_set2 << endl;

}