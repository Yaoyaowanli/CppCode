//
// Created by 遥远 on 2022/11/24.
//

#include <iostream>
#include "my_map.h"


void test_my_map(){
    yao::map<int,int> m;
    m.Insert(std::make_pair(1,1));
    m.Insert(std::make_pair(2,2));
    m.Insert(std::make_pair(3,3));
    m.Insert(std::make_pair(4,4));
    m.Insert(std::make_pair(5,5));
    m.Insert(std::make_pair(6,6));
    auto it = m.begin();
    while (it != m.end()){
        std::cout << (*it).first << " : " << (*it).second << std::endl;
        ++it;
    }
}

void test_my_map2() {
    std::string strs[] = {"a","b","c","a","c","a","a","b","c"};
    yao::map<std::string,int> m;
    for (auto& str:strs){
        m[str]++;
    }
    auto it = m.begin();
    while (it != m.end()){
        std::cout << (*it).first << " : " << (*it).second << std::endl;
        ++it;
    }
}