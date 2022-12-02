//
// Created by 遥远 on 2022/12/2.
//

#include "my_unordered_map.h"


void test_my_unordered_map_1(){
    yao::my_unordered_map<std::string,std::string> my_hash_map;
    my_hash_map.insert(std::make_pair("yaoyuan","yaoyuan"));
    my_hash_map.insert(std::make_pair("abc","abc"));
    my_hash_map.insert(std::make_pair("abcd","abcd"));
    my_hash_map.insert(std::make_pair("acbd","acbd"));
    my_hash_map.insert(std::make_pair("abbc","abbc"));

}

void test_my_unordered_map_2() {
    yao::my_unordered_map<int,int> my_hash_map;
    my_hash_map.insert(std::make_pair(1,1));
    my_hash_map.insert(std::make_pair(5,5));
    my_hash_map.insert(std::make_pair(4,4));
    my_hash_map.insert(std::make_pair(2,2));
    my_hash_map.insert(std::make_pair(12,12));
    my_hash_map.insert(std::make_pair(16,16));
    my_hash_map.insert(std::make_pair(7,7));
    my_hash_map[1] = 99;
    yao::my_unordered_map<int,int>::iterator it = my_hash_map.begin();
    while (it != my_hash_map.end()){
        std::cout << it->first << " : " << it->second << std::endl;
        ++it;
    }


}