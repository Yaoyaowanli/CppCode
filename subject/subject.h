//
// Created by 遥远 on 2022/12/4.
//

#ifndef CPPCODE_SUBJECT_H
#define CPPCODE_SUBJECT_H

#endif //CPPCODE_SUBJECT_H

#pragma once
#include <iostream>
#include <vector>
using namespace std;

//海量数据处理

//给定100亿个未排序的整数，设计算法找到只出现一次的整数
// 思路：100亿个整数，400亿字节=39062500kb=38146mb=37gb，内存显然不够。所以排除了全部放在内存中查找的可能。
// 解决方案： size_t(unsigned long)整数的范围最大在0~42亿多，43亿个整形也需要16G空间，我们可以采用位图，每一个比特位映射一个数
//          只需要约500M的空间就可以解决，但题目要求找到只出现1次的数，位图的1bit只能反应0和1两种状态，此时需要记录3种状态
//          出现0次，出现1次，出现2次及以上，可以将位图的扩大一下，2个bit位映射1个数，00表示没出现，01表示出现1次，10表示出现2次及以上
//          只需要1G的内存就可以解决。

class solution{
public:
    //使用2个位图来表示
    void set(size_t x){
      if (!_bs1.test(x) && !_bs2.test(x)) {
          //00
          _bs2.set(x); //01
      }else if (!_bs1.test(x) && _bs2.test(x)){
          //01
          _bs1.set(x);
          _bs2.reset(x);    //10
      }
      //10 不做处理
    }
    bool test_first(size_t x){
        //只有在2个位图中呈现01的数返回true，只要遍历100亿个数调用该接口，记录下为true的数即可。
        if (!_bs1.test(x) && _bs2.test(x)){
            return true;
        }
        return false;
    }

private:
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
    bitSet _bs1;
    bitSet _bs2;
};

//给两个文件，分别有100亿个整数，找到两个文件的交集
//解决方案： 利用上面的位图，先将第一个文件的所有数据映射到位图中，再遍历第二个文件，test查找，记录重复出现的数。




//给两个文件，分别有100亿个query，只有1G内存，如何找到两个文件的交集？（给出近似算法和精确算法）
// 近似算法：将文件1中的所有query映射到布隆过滤器，然后读取文件2的query，判断是否在布隆过滤器中。   缺陷：找到的交集中有些数据不准确。
// 精确算法：哈希切分，分别遍历文件A、B，读取query，i = hash_str(query) % 切割的文件个数   再把query写入Ai/Bi 中；
//         遍历写入完成后，相同的query一定都进入了i号文件(因为相同的query算出的hash值是相同的)；
//         再用set/unordered_set 在Ai/Bi中找交集即可。



//如果扩展bloomFiler 使得他支持删除？
//    在布隆过滤器中的标记位后面增加位表示计数器，有值映射该位后在该位后面的计数器位+1，删除通过-1来实现，但是会增加布隆的空间；
//    布隆过滤器的特点就是节省空间。