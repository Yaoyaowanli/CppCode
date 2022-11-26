//
// Created by 遥远 on 2022/11/24.
//

#ifndef CPPCODE_MY_MAP_H
#define CPPCODE_MY_MAP_H

#endif //CPPCODE_MY_MAP_H
#pragma once
#include <iostream>
#include "rb_tree.h"


namespace yao{
    template<class K,class V>
    class map{
    public:
        //这里是一个仿函数，实现map需要key，value，所以这里重载（），在红黑树中创建一个MapKeyOrT变量，传入的是pair就取出pair中的first
        struct MapKeyOrT{
            const K& operator() (const std::pair<K,V>& kv){
                return kv.first;
            }
        };
        //iterator 重命名
        typedef typename RBTree<K,std::pair<K,V>,MapKeyOrT>::iterator iterator;
        iterator begin(){
            //返回RBTree
            return _t.begin();
        }
        iterator end(){
            return _t.end();
        }
        std::pair<iterator ,bool> Insert(const std::pair<K,V>& kv){
            return _t.Insert(kv);
        }
        V& operator[](const K& key){
            std::pair<iterator,bool> ret = _t.Insert(std::make_pair(key,V()));
            return ret.first->second;
        }
    private:
        RBTree<K,std::pair<K,V>,MapKeyOrT> _t;
    };
}




void test_my_map();
void test_my_map2();
