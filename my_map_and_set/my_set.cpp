//
// Created by 遥远 on 2022/11/24.
//

#include "my_set.h"
void test_my_set(){
    yao::set<int> s;
    s.Insert(3);
    s.Insert(4);
    s.Insert(1);
    s.Insert(2);
    s.Insert(5);

    yao::set<int>::iterator it = s.begin();
    while (it != s.end()){
        std::cout << *it << " ";
        ++it;
    }
}