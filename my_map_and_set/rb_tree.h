//
// Created by 遥远 on 2022/11/24.
//

#ifndef CPPCODE_RB_TREE_H
#define CPPCODE_RB_TREE_H

#endif //CPPCODE_RB_TREE_H

#pragma once
#include <iostream>


// Red Black Tree

enum Colour {
    BLACK,
    RED,
};

//RBTreeNode
template<class T>
struct RBTreeNode{
    RBTreeNode<T>* _left;
    RBTreeNode<T>* _right;
    RBTreeNode<T>* _parent;
    T _data;
//    std::pair<K,V> _kv;         //kv键值对
    Colour _col;
    RBTreeNode(const T& data);
};

//RBTree iterator
//RBTree不是连续的内存空间，没有办法通过++--指针来控制访问，所以红黑树的iterator通过封装指针并且重载运算符定义其指针的移动方式来访问
template<class T,class Ref,class Ptr>
struct __TreeIterator{
    typedef RBTreeNode<T> Node;
    typedef __TreeIterator<T,Ref,Ptr> Self;
    Node* _node;

    explicit __TreeIterator(Node* node)
            :_node(node)
    {}
    Ref operator*();
    Ptr operator->();
    Self& operator++();
    Self& operator--();
    bool operator!=(const Self& s){
        return _node != s._node;
    }
    //bool operator==(const Self& s);

};


//RBTree
template<class K,class T,class KOfT>
class RBTree{
    typedef RBTreeNode<T> Node;
public:
    typedef __TreeIterator<T,T&,T*> iterator;
    typedef __TreeIterator<T,const T&,const T*> const_iterator;
    iterator begin();
    iterator end();
    std::pair<iterator,bool> Insert(const T& data);
    void InOrder()const;
    bool IsValidRBTree();
    iterator Find(const K& key);
private:
    bool _IsValidRBTree(RBTreeNode<T>* root,size_t k,size_t BlackNode_count);
    void rotateL(Node* parent);
    void rotateR(Node* parent);
    void _InOrder(Node *root)const;
    Node* _root = nullptr;
};


// RBTreeNode
template<class T>
RBTreeNode<T>::RBTreeNode(const T& data)
        :_right(nullptr),
         _parent(nullptr),
         _left(nullptr),
         _data(data),
         _col(RED)
{}


//RBTree
template<class K,class T,class KOfT>
__TreeIterator<T,T&,T*> RBTree<K,T,KOfT>::begin() {
    Node* cur = _root;
    Node* parent = nullptr;
    while (cur){
        parent = cur;
        cur = cur->_left;
    }
    return __TreeIterator<T,T&,T*>(parent);
}

template<class K,class T,class KOfT>
__TreeIterator<T,T&,T*> RBTree<K,T,KOfT>::end() {
    return __TreeIterator<T,T&,T*>(nullptr);
}

template<class K,class T,class KOfT>
std::pair<__TreeIterator<T,T&,T*>,bool> RBTree<K, T,KOfT>::Insert(const T& data) {
    //1、按照二叉搜索树的规则插入
    //1.1、如果当前红黑树为空树，构造节点为root
    if (_root == nullptr){
        _root = new Node(data);
        _root->_col = BLACK;
        return std::make_pair(__TreeIterator<T,T&,T*>(_root),true);
    }
    //1.1、如果不为空，则通过比较pair.first的键来确定最终插入的位置
    KOfT kOft;
    Node* cur = _root;
    Node* parent = nullptr;
    while (cur){
        if (kOft(cur->_data) > kOft(data)){
            parent = cur;
            cur = cur->_left;
        }else if (kOft(cur->_data) < kOft(data)){
            parent = cur;
            cur = cur->_right;
        } else {
            return std::make_pair(__TreeIterator<T,T&,T*>(cur),false);
        }
    }
    //1.2、cur已经找到了空位置,判断cur应该插入在parent的左边还是右边.
    cur = new Node(data);
    Node* newNode = cur;
    if (kOft(parent->_data) > kOft(data)){
        parent->_left = cur;
        cur ->_parent = parent;
    }else{
        parent->_right = cur;
        cur->_parent = parent;
    }
    //新插入的节点默认为红色，因为默认为黑色的话会改变路径上黑色节点的数量，不好控制
    cur->_col = RED;
    //如果新插入节点cur的parent是红色，那么他的grandfather一定是黑色，这个时候我们通过变色来保持红黑树
    if (parent == _root){
        return std::make_pair(__TreeIterator<T,T&,T*>(cur),true);
    }
    while (parent && parent->_col == RED){
        //红黑树的调节关键看uncle
        Node* grandfather = parent->_parent;
        if (grandfather->_left == parent){
            //parent 在 grandfather的左子树，uncle在右子树的大情况：
            Node* uncle = grandfather->_right;
            //情况1：uncle存在且为红，就将parent和uncle变黑，grandfather变红
            if (uncle && uncle->_col == RED){
                parent->_col = BLACK;
                uncle->_col = BLACK;
                grandfather->_col = RED;
                //迭代
                cur = grandfather;
                parent = grandfather->_parent;
            }else{
                //情况2 or 情况3 ： uncle不存在 ｜｜ uncle存在且为黑
                //情况3：双旋->变为单旋
                //这里parent在grandfather的左子树，如果cur在parent的右边，就需要做左右双旋，这里先做了左旋，再交换了parent和cur
                //将双旋的情况转换为单弦
                if (cur == parent->_right){
                    rotateL(parent);
                    std::swap(parent,cur);
                }

                //第二种情况（有可能是第三种情况变过来的）
                rotateR(grandfather);
                grandfather->_col = RED;
                parent->_col = BLACK;
                break;
            }
        }else{
            //parent 在 grandfather的右子树，uncle在左子树的大情况：
            Node* uncle = grandfather->_left;
            //情况1：uncle存在且为红，就将parent和uncle变黑，grandfather变红
            if (uncle && uncle->_col == RED){
                parent->_col = BLACK;
                uncle->_col = BLACK;
                grandfather->_col = RED;
                //迭代
                cur = grandfather;
                parent = grandfather->_parent;
            }else{
                //情况2 or 情况3 ：uncle不存在 ｜｜ uncle存在且为黑
                //这里parent在grandfather的右子树，如果cur在parent的左边，就需要做右左双旋，这里先做了右旋，再交换了parent和cur
                //将双旋的情况转换为单弦
                if (cur == parent->_left){
                    rotateR(parent);
                    std::swap(parent,cur);
                }

                //第二种情况（有可能是第三种情况变过来的）
                rotateL(grandfather);
                grandfather->_col = RED;
                parent->_col = BLACK;
                break;
            }
        }
    }

    _root->_col = BLACK;
    return std::make_pair(__TreeIterator<T,T&,T*>(newNode), true);
}

template<class K,class T,class KOfT>
void RBTree<K,T,KOfT>::rotateL(Node *parent) {
    //左单旋
    Node* ppNode = parent->_parent;     //提前保存parent的父节点，有可能是null
    Node* subR = parent->_right;
    Node* subRL = subR->_left;
    //首先让parent的right(原本指向的是subR)指向subR的左边（也就是subRL）
    parent->_right = subRL;
    //这里subRL有可能是nullptr
    if (subRL){
        //如果不为空，就让subRL的父指向parent
        subRL->_parent = parent;
    }
    subR->_left = parent;
    parent->_parent = subR;
    //到这里已经完成了问题子树的旋转。但是问题子树不代表整颗AVL树，上面可能还有节点
    //有两种情况
    //1、parent就是整颗树的root节点，旋转完成后subR应该成为新的root节点
    if (_root == parent){
        _root = subR;
        subR->_parent = nullptr;
    }else{
        //2、parent不是整棵树的root，那么subR就应该指向最开始parent-》parent，且parent-》parent 应该指向subR
        if (ppNode->_left == parent){
            ppNode->_left = subR;
        }else{
            ppNode->_right = subR;
        }
        subR->_parent = ppNode;
    }
}

template<class K,class T,class KOfT>
void RBTree<K,T,KOfT>::rotateR(Node *parent) {
    //右单旋
    Node* ppNode = parent->_parent;
    Node* subL = parent->_left;
    Node* subLR = subL->_right;

    parent->_left = subLR;
    if (subLR){
        subLR->_parent = parent;
    }
    subL->_right = parent;
    parent->_parent = subL;

    if (_root == parent){
        _root = subL;
        subL->_parent = nullptr;
    }else{
        if (ppNode->_left == parent){
            ppNode->_left = subL;
        }else{
            ppNode->_right = subL;
        }
        subL->_parent = ppNode;
    }
}

template<class K,class T,class KOfT>
void RBTree<K,T,KOfT>::InOrder()const {
    _InOrder(_root);
    std::cout << std::endl;
}

template<class K,class T,class KOfT>
void RBTree<K,T,KOfT>::_InOrder(Node *root)const{
    if (root == nullptr){
        return;
    }
    _InOrder(root->_left);
    //std::cout << root->_kv.first << " : " << root->_kv.second << std::endl;
    _InOrder(root->_right);
}

template<class K,class T,class KOfT>
bool RBTree<K,T,KOfT>::IsValidRBTree() {
    //检测一棵树是不是红黑树：
    //1、检测其中续遍历是否是有序的
    //2、检测其是否满足红黑树的性质
    //空树也是红黑树
    if (_root == nullptr){
        return true;
    }
    //红黑树的根节点必须是黑色
    if(_root->_col != BLACK){
        std::cout << "error: _root->_col must be BLACK !" << std::endl;
        return false;
    }
    //获取任意一条路径中的黑色节点数量
    size_t BlackNode_count = 0;
    Node* cur = _root;
    while(cur){
        if (cur->_col == BLACK){
            BlackNode_count++;
        }
        cur = cur->_left;
    }
    //k用来记录路径中黑色节点的个数
    size_t k = 0;
    return _IsValidRBTree(_root,k,BlackNode_count);
}


template<class K,class T,class KOfT>
bool RBTree<K,T,KOfT>::_IsValidRBTree(RBTreeNode<T>* root,size_t k,size_t BlackNode_count) {
    //走到null之后，判断k和blackNode_count是否相等
    if (root == nullptr){
        if (k != BlackNode_count){
            std::cout << "error : 路径中黑色节点的数量不一致" << std::endl;
            return false;
        }
        return true;
    }

    //统计黑色节点数量
    if (root->_col == BLACK){
        k++;
    }

    //当前节点如果为红色，检测其父亲节点是否为红色
    Node* parent = root->_parent;
    if (parent && parent->_col == RED && root->_col == RED){
        std::cout << "error：parent->col == cur->_col == RED ！" << std::endl;
        return false;
    }

    return _IsValidRBTree(root->_left,k,BlackNode_count) && _IsValidRBTree(root->_right,k,BlackNode_count);

}

template<class K,class T,class KOfT>
__TreeIterator<T,T&,T*> RBTree<K,T,KOfT>::Find(const K& key) {
    KOfT kOft;
    Node* cur = _root;
    while (cur){
        if (kOft(cur->_data) > key){
            cur = cur->_left;
        }else if (kOft(cur->_data) < key){
            cur = cur->_right;
        }else{
            return __TreeIterator<T,T&,T*>(cur);
        }
    }
    return __TreeIterator<T,T&,T*>(nullptr);
}

//RBTree iterator
template <class T,class Ref,class Ptr>
Ref  __TreeIterator<T,Ref,Ptr>::operator*() {
    return _node->_data;
}

template<class T,class Ref,class Ptr>
Ptr __TreeIterator<T,Ref,Ptr>::operator->() {
    return &_node->_data;
}

template<class T,class Ref,class Ptr>
__TreeIterator<T,Ref,Ptr>& __TreeIterator<T,Ref,Ptr>::operator++() {
    //如果右不为空，则节点就应该移动到右树的最左节点
    if (_node->_right){
        Node* subLeft = _node->_right;
        while (subLeft->_left){
            subLeft = subLeft->_left;
        }
        _node = subLeft;
    }else{
        //如果右为空，表示_node所在的子树访问完了，需要移动到其祖先中
        Node* cur = _node;
        Node* parent = cur->_parent;
        while (parent && cur == parent->_right){
            cur = cur->_parent;
            parent = parent->_parent;
        }
        _node = parent;
    }
    return *this;
}

template<class T,class Ref,class Ptr>
__TreeIterator<T,Ref,Ptr>& __TreeIterator<T,Ref,Ptr>::operator--() {

}