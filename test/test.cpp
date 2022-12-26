//
// Created by 遥远 on 2022/12/26.
//

#include "test.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

void test_1(){
    std::vector<int> v = {5,2,3,1,4};

    //全排列的vector要求有序
    sort(v.begin(),v.end());

    std::vector<int>::iterator it = v.begin();
    while(it != v.end()){
        std::cout << *it << "  ";
        ++it;
    }
    std::cout << std::endl;

    std::cout << "---------------------------------"<<std::endl;

    //生序
    do{
        for (auto& e:v){

            std::cout << e << " ";

        }
        std::cout << std::endl;
    }while(next_permutation(v.begin(),v.end()));

    std::cout << "---------------------------------"<<std::endl;
    //降序

    sort(v.begin(),v.end());

    do {
        for (auto& e:v){
            std::cout << e << " ";
        }
        std::cout << std::endl;
    }while(prev_permutation(v.begin(),v.end()));


    return 0;
}