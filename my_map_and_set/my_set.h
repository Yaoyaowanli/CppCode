//
// Created by 遥远 on 2022/11/24.
//

#ifndef CPPCODE_MY_SET_H
#define CPPCODE_MY_SET_H

#endif //CPPCODE_MY_SET_H
#pragma once
#include <iostream>
#include "rb_tree.h"


namespace yao{
    template<class K>
    class set{
    public:
        //这里是一个仿函数，实现map需要key，value，所以这里重载（），在红黑树中创建一个MapKeyOrT变量，传入的是pair就取出pair中的first
        //如果是set，就取出k本身
        struct SetOfT{
            const K& operator() (const K& k){
                return k;
            }
        };
        typedef typename RBTree<K,K,SetOfT>::iterator iterator;
        iterator begin(){
            return _t.begin();
        }
        iterator end(){
            return _t.end();
        }

        std::pair<iterator,bool> Insert(const K& k){
            return _t.Insert(k);
        }

    private:
        RBTree<K,K,SetOfT> _t;
    };

}

void test_my_set();

