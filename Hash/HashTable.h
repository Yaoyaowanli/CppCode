//
// Created by 遥远 on 2022/11/27.
//

#ifndef CPPCODE_HASHTABLE_H
#define CPPCODE_HASHTABLE_H

#endif //CPPCODE_HASHTABLE_H

#pragma once
#include <iostream>
#include <vector>


// HashTable    闭散列线性探测
//unordered_set和unordered_map共用一个hash table 代码：
//unordered_set<K>    ->  HashTable<K,K>
//unordered_map<K,V>  ->  HashTable<K,pair<K,V>>

//State表示HashData中的状态，用于解决哈希冲突时数据如何查找。
enum State{
    EMPTY,      //该位未存储过数据,查找冲突数据到这里就说明没有该数据。
    EXITS,      //该位存储了一个有效数据，继续向后查找。
    DELETE,     //该位曾经删除了一个有效数据，继续向后查找。
};

template<class T>
struct HashData{
    T _data;
    State _state;
};

template<class K,class T,class KOfT>
class HashTable{
public:
    typedef HashData<T> HashData;
    bool Insert(const T& d);
    HashData* Find (const K& key);
    bool Erase (const K& key);
private:
    std::vector<HashData> _tables;
    size_t _num;                //存了多少有效数据

};


//insert：
// insert根据数据d中的key 来计算index，如果index位置产生冲突，就进行线性探测向后找，直到找到EMPTY位置插入。
template<class K,class T,class KOfT>
bool HashTable<K,T,KOfT>::Insert(const T &d) {
    KOfT kOft;
    //负载因子 = 表中数据/表的大小 ,衡量哈希表满的程度
    //表越接近满，插入数据越容易冲突，冲突越多，效率越低
    //哈希表并不是满了才增容，开放地址法中，一般负载因子达到0.7左右就开始增容
    //负载因子越小，冲突概率越低，整体效率越高，但是空间浪费越多。
    if (_tables.size() == 0 || _num*10 /_tables.size() >= 7){
        //增容
        std::vector<HashData> new_table;
        //避免除0 错误
        size_t new_size = _tables.size()==0 ? 10:_tables.size()*2;
        new_table.resize(new_size);
        for (size_t i=0; i<_tables.size(); i++){
            //重新计算旧表中每个值的散列
            if (_tables[i]._state == EXITS){
                size_t index = kOft(_tables[i]._data) % new_table.size();
                while (new_table[index]._state == EXITS){
                    ++index;
                    if (index == new_table.size()){
                        index = 0;
                    }
                }
                new_table[index]._data = _tables[i];
                new_table[index]._state = EXITS;
            }
        }
        //交换，旧表数据就swap到了new_table里，出了作用域就会自动析构。
        _tables.swap(new_table);
    }

    //计算d中的key在表中映射的位置
    size_t index = kOft(d) % _tables.size();
    //找到index位置，如果冲突就向后移动
    while (_tables[index]._state == EXITS){
        if (_tables[index]._data == kOft(d) ){
            return false;
        }
        ++index;
        //如果index走完了，就置为0开始
        if (index == _tables.size()){
            index = 0;
        }
    }
    _tables[index]._data = d;
    _tables[index]._state = EXITS;
    _num++;
    return true;
}

template<class K,class T,class KOfT>
HashData<T>* HashTable<K,T,KOfT>::Find(const K &key) {
    KOfT kOft;
    size_t index = key % _tables.size();
    while (_tables[index]._state != EMPTY){
        if (kOft(_tables[index]._data) == key){
            if (_tables[index]._state == EXITS){
                return &_tables[index];
            }else if (_tables[index]._state == DELETE){
                return nullptr;
            }
        }
        ++index;
        if (index == _tables.size()){
            index = 0;
        }
    }
    return nullptr;
}

template<class K,class T,class KOfT>
bool HashTable<K,T,KOfT>::Erase(const K &key) {
    HashTable<K,T,KOfT>::HashData* ret = Find(key);
    if (ret){
        ret->_state = DELETE;
        _num--;
        return true;
    }
    return false;
}