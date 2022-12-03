//
// Created by 遥远 on 2022/12/3.
//

#ifndef CPPCODE_BLOOMFILTER_H
#define CPPCODE_BLOOMFILTER_H

#endif //CPPCODE_BLOOMFILTER_H

#pragma once
#include <iostream>
#include "bit_set.h"


//布隆过滤器
namespace yao{

    struct Hash_str_1{
        //BKDR HASH
      size_t operator()(const std::string& str){
          size_t hash = 0;
          for (size_t i=0; i<str.size(); i++){
              hash *= 131;
              hash += str[i];
          }
          return hash;
      }
    };

    struct Hash_str_2{
        //RS Hash
        size_t operator()(const std::string& str){
            size_t hash = 0;
            size_t magic = 63689;
            for (size_t i=0; i<str.size(); i++){
                hash *= magic;
                hash += str[i];
                magic *= 378551;
            }
            return hash;
        }
    };

    struct Hash_str_3{
        //SDBM Hash
        size_t operator()(const std::string& str){
            size_t hash = 0;
            for (size_t i=0; i<str.size(); i++){
                hash *= 65599;
                hash += str[i];
            }
            return hash;
        }
    };

    template<class K = std::string ,class Hash_1 = Hash_str_1,class Hash_2 = Hash_str_2,class Hash_3 = Hash_str_3>
    class bloom_filter{
    public:
        bloom_filter(size_t num)
        : _bit_set(5*num)
        ,_N(5*num)
        {}
        void set(const K& key){
            size_t index_1 = Hash_1()(key) % _N;
            size_t index_2 = Hash_2()(key) % _N;
            size_t index_3 = Hash_3()(key) % _N;

            _bit_set.set(index_1);
            _bit_set.set(index_2);
            _bit_set.set(index_3);
        }

        bool test(const K& key){
            size_t index_1 = Hash_1()(key) % _N;
            if (! _bit_set.test(index_1)){
                return false;
            }
            size_t index_2 = Hash_2()(key) % _N;
            if (! _bit_set.test(index_2)){
                return false;
            }
            size_t index_3 = Hash_3()(key) % _N;
            if (! _bit_set.test(index_3)){
                return false;
            }

            return true;        //但是这里也不一定是一定在，还是可能存在误判，算三个哈希值就是位了降低误判，不在是准确的，在不一定。
        }

        //布隆过滤器一般不支持reset，因为set时使用了好几个哈希函数，reset会导致其他的映射也被删除
        //void reset(const K& key){}
    private:
        bitSet _bit_set;    //位图
        size_t _N;
    };
}