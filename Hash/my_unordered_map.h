//
// Created by 遥远 on 2022/12/2.
//

#ifndef CPPCODE_MY_UNORDERED_MAP_H
#define CPPCODE_MY_UNORDERED_MAP_H

#endif //CPPCODE_MY_UNORDERED_MAP_H

#pragma once
#include "HashTable.h"


namespace yao{

    template<class K,class V,class Hash = yao_open_hash::_Hash<K>>
    class my_unordered_map{
        struct MapKOfT{
            const K& operator()(const std::pair<K,V>& kv){
                return kv.first;
            }
        };
    public:
        typedef typename yao_open_hash::HashTable<K,std::pair<K,V>,MapKOfT,Hash>::iterator iterator;
        std::pair<iterator,bool> insert(const std::pair<K,V>& kv);
        iterator begin(){
            return _ht.Begin();
        }
        iterator end(){
            return _ht.End();
        }
        V& operator[](const K& key){
            std::pair<iterator,bool>ret = _ht.Insert(std::make_pair(key,V()));
            return ret.first->second;
        }
    private:
        yao_open_hash::HashTable<K,std::pair<K,V>,MapKOfT,Hash> _ht;
    };

    template<class K,class V,class Hash>
    std::pair<typename my_unordered_map<K,V,Hash>::iterator,bool>
    my_unordered_map<K,V,Hash>::insert(const std::pair<K, V> &kv) {
        return _ht.Insert(kv);
    }

}






void test_my_unordered_map_1();
void test_my_unordered_map_2();