//
// Created by 遥远 on 2022/11/27.
//

#ifndef CPPCODE_HASHTABLE_H
#define CPPCODE_HASHTABLE_H

#endif //CPPCODE_HASHTABLE_H

#pragma once
#include <iostream>
#include <vector>

namespace yao_close_hash {
// HashTable    闭散列线性探测
//unordered_set和unordered_map共用一个hash table 代码：
//unordered_set<K>    ->  HashTable<K,K>
//unordered_map<K,V>  ->  HashTable<K,pair<K,V>>

//State表示HashData中的状态，用于解决哈希冲突时数据如何查找。
    enum State {
        EMPTY,      //该位未存储过数据,查找冲突数据到这里就说明没有该数据。
        EXITS,      //该位存储了一个有效数据，继续向后查找。
        DELETE,     //该位曾经删除了一个有效数据，继续向后查找。
    };

    template<class T>
    struct HashData {
        T _data;
        State _state;
    };

    template<class K, class T, class KOfT>
    class HashTable {
    public:
        typedef HashData<T> HashData;

        bool Insert(const T &d);

        HashData *Find(const K &key);

        bool Erase(const K &key);

    private:
        std::vector<HashData> _tables;
        size_t _num;                //存了多少有效数据

    };


//insert：
// insert根据数据d中的key 来计算index，如果index位置产生冲突，就进行线性探测向后找，直到找到EMPTY位置插入。
    template<class K, class T, class KOfT>
    bool HashTable<K, T, KOfT>::Insert(const T &d) {
        KOfT kOft;
        //负载因子 = 表中数据/表的大小 ,衡量哈希表满的程度
        //表越接近满，插入数据越容易冲突，冲突越多，效率越低
        //哈希表并不是满了才增容，开放地址法中，一般负载因子达到0.7左右就开始增容
        //负载因子越小，冲突概率越低，整体效率越高，但是空间浪费越多。
        if (_tables.size() == 0 || _num * 10 / _tables.size() >= 7) {
            //增容
            std::vector<HashData> new_table;
            //避免除0 错误
            size_t new_size = _tables.size() == 0 ? 10 : _tables.size() * 2;
            new_table.resize(new_size);
            for (size_t i = 0; i < _tables.size(); i++) {
                //重新计算旧表中每个值的散列
                if (_tables[i]._state == EXITS) {
                    size_t index = kOft(_tables[i]._data) % new_table.size();
                    while (new_table[index]._state == EXITS) {
                        ++index;
                        if (index == new_table.size()) {
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
        while (_tables[index]._state == EXITS) {
            if (_tables[index]._data == kOft(d)) {
                return false;
            }
            ++index;
            //如果index走完了，就置为0开始
            if (index == _tables.size()) {
                index = 0;
            }
        }
        _tables[index]._data = d;
        _tables[index]._state = EXITS;
        _num++;
        return true;
    }

    template<class K, class T, class KOfT>
    HashData<T> *HashTable<K, T, KOfT>::Find(const K &key) {
        KOfT kOft;
        size_t index = key % _tables.size();
        while (_tables[index]._state != EMPTY) {
            if (kOft(_tables[index]._data) == key) {
                if (_tables[index]._state == EXITS) {
                    return &_tables[index];
                } else if (_tables[index]._state == DELETE) {
                    return nullptr;
                }
            }
            ++index;
            if (index == _tables.size()) {
                index = 0;
            }
        }
        return nullptr;
    }

    template<class K, class T, class KOfT>
    bool HashTable<K, T, KOfT>::Erase(const K &key) {
        HashTable<K, T, KOfT>::HashData *ret = Find(key);
        if (ret) {
            ret->_state = DELETE;
            _num--;
            return true;
        }
        return false;
    }
}



namespace yao_open_hash {
    //HashTable 开散列，哈希桶
    template<class T>
    struct HashNode{
        typedef HashNode<T> Node;
        T _data;
        Node* _next;
        explicit HashNode(const T& data);
    };

    template<class K,class T,class KOfT>
    class HashTable{
    typedef HashNode<T> Node;
    public:
        bool Insert(const T& data);
        Node* Find(const K& key);
        bool Erase(const K& key);
    private:
        std::vector<Node*> _tables;
        size_t _num = 0;                    //表中存储的数据个数
    };


    template<class T>
    HashNode<T>::HashNode(const T& data)
    :_data(data)
    ,_next(nullptr)
    {}


    template<class K,class T,class KOfT>
    bool HashTable<K,T,KOfT>::Insert(const T &data) {
        //开散列当插入数据遇到哈希冲突时，会将冲突数据链接到当前数据后，哈希表中的每一个元素都是一个链表，不过当大量冲突发生时
        //这些冲突的数据就会挂在同一个链式桶中，链表过长，就会导致查找效率低下；
        //所以开散列的哈希桶也是需要控制其负载的，我们一般把散列负载因子控制在1。
        KOfT k_of_t;
        //检查、扩容
        if (_tables.size() == _num){
            std::vector<Node*> new_tables;
            size_t new_size = _tables.size()==0 ? 10 : _tables.size()*2;
            new_tables.resize(new_size);
            for (size_t i=0; i<_tables.size(); i++){
                //取到每一个桶
                Node* cur = _tables[i];
                while (cur){
                    Node* cur_next = cur->_next;
                    //计算桶中该元素在新表中的index,链接
                    size_t index = k_of_t(cur->_data) % new_tables.size();
                    cur->_next = new_tables[index];
                    new_tables[index] = cur;
                    //迭代当前桶
                    cur = cur_next;
                }
                //旧表该位置置空，防止析构节点
                _tables[i] = nullptr;
            }
            _tables.swap(new_tables);
        }
        //找这个值是否存在
        size_t index = k_of_t(data) % _tables.size();
        Node* cur = _tables[index];
        while (cur){
            if (k_of_t(cur->_data) == k_of_t(data)){
                return false;
            }else{
                cur = cur->_next;
            }
        }
        //不存在、插入
        Node* new_node = new Node(data);
        new_node->_next = _tables[index];
        _tables[index] = new_node;
        ++_num;
        return true;
    }

    template<class K,class T,class KOfT>
    HashNode<T>* HashTable<K,T,KOfT>::Find(const K &key) {
        KOfT k_of_t;
        size_t index = key % _tables.size();
        Node* cur = _tables[index];
        while (cur){
            if (k_of_t(cur->_data) == key){
                return cur;
            }else{
                cur = cur->_next;
            }
        }
        return nullptr;
    }

    template<class K,class T,class KOfT>
    bool HashTable<K,T,KOfT>::Erase(const K &key) {
        KOfT k_of_t;
        size_t index = key % _tables.size();
        Node* delete_node;
        Node* cur = _tables[index];
        //如果index位置就是要删除的数据
        if (k_of_t(cur->_data) == key){
            delete_node = cur;    //记录要删除的节点
            cur = cur->_next;           //cur走到下一个
            delete delete_node;
            _tables[index] = cur;       //重新赋值桶首
            _num--;
            return true;
        }
        while (cur){
            //如果下一个节点是要删除的节点
            if (k_of_t((cur->_next)->_data) == key){
                delete_node = cur->_next;       //记录要删除的节点
                cur->_next = delete_node->_next;    //cur->next 变为 删除节点的下一个
                delete delete_node;
                _num--;
                return true;
            }else{
                cur=cur->_next;
            }
        }
        return false;
    }

}