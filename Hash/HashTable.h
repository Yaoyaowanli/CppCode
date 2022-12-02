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




//-----------------------------------------------------------------------------------



namespace yao_open_hash {
    //HashTable 开散列，哈希桶

    //对于普通的类型，比如int等可以在散列中直接取模的，就直接返回他
    template<class K>
    struct _Hash{
        const K& operator()(const K& key){
            return key;
        }
    };

// 特化，如果是string 没法直接取模的，需要用字符串哈希算法处理
    template<>
    struct _Hash<std::string>{
        size_t operator()(const std::string& str){
            //BKDR Hash
            size_t hash = 0;
            for(size_t i=0;i<str.size();i++){
                hash *= 131;
                hash += str[i];
            }
            return hash;
        }
    };


    template<class T>
    struct HashNode{
        typedef HashNode<T> Node;
        T _data;
        Node* _next;
        explicit HashNode(const T& data);
    };


    //前置声明： 下面的__HashTable_Iterator和HashTable是互相依赖的，__HashTable_Iterator里需要有HashTable成员
    //HashTable 里需要typedef__HashTable_Iterator，谁在前谁在后都无法通过编译
    //故需要提前声明一下HashTable让编译器知道有这个类，让iterator可以使用
    template<class K,class T,class KOfT,class Hash>
    class HashTable;


    template<class K,class T,class KOfT,class Hash>
    struct __HashTable_Iterator{
        typedef HashNode<T> Node;
        typedef __HashTable_Iterator<K,T,KOfT,Hash> Self;
        typedef HashTable<K,T,KOfT,Hash> HashTable;
        Node* _node;
        HashTable* _pht;

        explicit __HashTable_Iterator(Node* node,HashTable* pht);
        T& operator*();
        T* operator->();
        Self operator++();
        bool operator!=(const Self& s);
    };
    
    template<class K,class T,class KOfT,class Hash = _Hash<K> >
    class HashTable{
    typedef HashNode<T> Node;
    public:
        //__HashTable_Iterator::operator++ 需要访问私有成员_tables。
        friend struct __HashTable_Iterator<K,T,KOfT,Hash>;
        typedef __HashTable_Iterator<K,T,KOfT,Hash> iterator;
            ~HashTable();
        void Clear();
        std::pair<iterator,bool> Insert(const T& data);
        Node* Find(const K& key);
        bool Erase(const K& key);
        size_t HashFunc(const K& key);
    //iterator:
        iterator Begin();
        iterator End();
    private:
        std::vector<Node*> _tables;
        size_t _num = 0;                    //表中存储的数据个数
    };




    template<class T>
    HashNode<T>::HashNode(const T& data)
    :_data(data)
    ,_next(nullptr)
    {}

    template<class K,class T,class KOfT,class Hash>
    HashTable<K,T,KOfT,Hash>::~HashTable() {
        Clear();
    }


    template<class K,class T,class KOfT,class Hash>
    size_t HashTable<K,T,KOfT,Hash>::HashFunc(const K& key) {
        //KOfT 彷函数 会把类型T中的key取出来
        //当set时，类型T就是类型K，可以直接返回，当map时，类型T就是一个pair<K,V>，他会取出其中的K返回。
        //这里的HashFunc,会处理key，当key是可以直接取模的int等类型时，直接返回，当是string时，没法直接取模，所以特化了一个string的处理
        //使用BKDR Hash 字符串哈希处理，让其转化成可以取模的size_t
        Hash hash;
        return hash(key);
    }

    template<class K,class T,class KOfT,class Hash>
    void HashTable<K,T,KOfT,Hash>::Clear() {
        for (size_t i=0; i<_tables.size(); i++){
            Node* cur = _tables[i];
            while(cur){
                Node* next = cur->_next;
                delete cur;
                cur = next;
            }
            _tables[i] = nullptr;
        }
    }

    template<class K,class T,class KOfT,class Hash>
    std::pair<__HashTable_Iterator<K,T,KOfT,Hash>,bool> HashTable<K,T,KOfT,Hash>::Insert(const T &data) {
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
                    size_t index = HashFunc(k_of_t(cur->_data)) % new_tables.size();
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
        size_t index = HashFunc(k_of_t(data)) % _tables.size();
        Node* cur = _tables[index];
        while (cur){
            if (k_of_t(cur->_data) == k_of_t(data)){
                return make_pair(iterator(cur,this),false);
            }else{
                cur = cur->_next;
            }
        }
        //不存在、插入
        Node* new_node = new Node(data);
        new_node->_next = _tables[index];
        _tables[index] = new_node;
        ++_num;
        return make_pair(iterator(new_node, this),true);
    }

    template<class K,class T,class KOfT,class Hash>
    HashNode<T>* HashTable<K,T,KOfT,Hash>::Find(const K &key) {
        KOfT k_of_t;
        size_t index = HashFunc(key) % _tables.size();
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

    template<class K,class T,class KOfT,class Hash>
    bool HashTable<K,T,KOfT,Hash>::Erase(const K &key) {
        KOfT k_of_t;
        size_t index = HashFunc(key) % _tables.size();
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

    //HashTable iterator:
    template<class K,class T,class KOfT,class Hash>
    __HashTable_Iterator<K,T,KOfT,Hash> HashTable<K,T,KOfT,Hash>::Begin() {
        for (size_t i=0; i<_tables.size();i++){
            if (_tables[i]){
                return iterator(_tables[i], this);
            }
        }

        return End();
    }

    template<class K,class T,class KOfT,class Hash>
    __HashTable_Iterator<K,T,KOfT,Hash> HashTable<K,T,KOfT,Hash>::End(){
        return iterator(nullptr, this);
    }

    //iterator
    template<class K,class T,class KOfT,class Hash>
    __HashTable_Iterator<K,T,KOfT,Hash>::__HashTable_Iterator(Node* node,HashTable* pht)
    :_node(node)
    ,_pht(pht)
    {}

    template<class K,class T,class KOfT,class Hash>
    T& __HashTable_Iterator<K, T, KOfT,Hash>::operator*() {
        return _node->_data;
    }

    template<class K,class T,class KOfT,class Hash>
    T *__HashTable_Iterator<K, T, KOfT,Hash>::operator->() {
        return &_node->_data;
    }

    template<class K,class T,class KOfT,class Hash>
    __HashTable_Iterator<K,T,KOfT,Hash> __HashTable_Iterator<K,T,KOfT,Hash>::operator++() {
        KOfT k_of_t;
        if (_node->_next){
            _node=_node->_next;
        }else{
            size_t i = _pht->HashFunc(k_of_t(_node->_data)) % _pht->_tables.size();
            ++i;
            for (;i<_pht->_tables.size();i++){
                Node* cur = _pht->_tables[i];
                if (cur){
                    _node = cur;
                    return *this;
                }
            }
            _node = nullptr;
        }
        return *this;
    }

    template<class K,class T,class KOfT,class Hash>
    bool __HashTable_Iterator<K,T,KOfT,Hash>::operator!=(const Self &s) {
        return _node != s._node;
    }


}

