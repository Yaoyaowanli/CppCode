//
// Created by 遥远 on 2022/12/3.
//

#ifndef CPPCODE_BIT_SET_H
#define CPPCODE_BIT_SET_H

#endif //CPPCODE_BIT_SET_H
#pragma once
#include <iostream>
#include <vector>

//位图

namespace yao{

    class bitSet{
    public:
        bitSet(size_t N){
            _bits.resize(N/32+1,0);
            _num = 0;
        }
        void set(size_t x){
            //计算x所在的整形，在计算其映射在哪个位，将该位置为1。
            size_t index = x / 32; //第几个整形
            size_t pos = x % 32;   //第几个位
            _bits[index] |= (1<<pos);   //第pos位置为1
            ++_num;
        }
        void reset(size_t x){
            size_t index = x / 32;
            size_t pos = x % 32;
            _bits[index] &= ~(1 << pos);
            --_num;
        }
        bool test(size_t x){
            size_t index = x / 32;
            size_t pos = x % 32;
            return _bits[index] & (1<<pos);
        }

    private:
        std::vector<int> _bits;
        size_t _num;  //映射存储的多少个数据
    };

}

void test_bitset_1();
void test_bitset_2();
