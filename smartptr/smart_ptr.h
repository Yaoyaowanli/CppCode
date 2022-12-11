//
// Created by 遥远 on 2022/12/9.
//

#ifndef CPPCODE_SMART_PTR_H
#define CPPCODE_SMART_PTR_H

#endif //CPPCODE_SMART_PTR_H

#pragma once
#include <iostream>

namespace yao{

    template<typename T>
    class SmartPtr{
    public:
        SmartPtr(T* ptr)
                :_ptr(ptr)
        {}

        ~SmartPtr(){
            if (_ptr){
                std::cout << "free: "<< _ptr << std::endl;
                delete _ptr;
            }
        }

        T& operator*(){
            return *_ptr;
        }

        T* operator->(){
            return _ptr;
        }

    private:
        T* _ptr;
    };


//c++98 auto_ptr 管理权转移(不好)
    template<typename T>
    class auto_ptr{
    public:
        auto_ptr(T* ptr)
                :_ptr(ptr)
        {}
        ~auto_ptr(){
            if (_ptr){
                std::cout<< "delete: "<< _ptr << std::endl;
                delete _ptr;
                _ptr = nullptr;
            }
        }

        //管理权转移，就是拷贝构造函数拷贝之后，将自己置空。避免重复释放空间
        auto_ptr(auto_ptr<T>& p)
                :_ptr(p._ptr)
        {
            p._ptr = nullptr;
        }
        auto_ptr<T>& operator=(const auto_ptr<T>& ap){
            if (*this != ap){
                if (_ptr){
                    delete _ptr;
                }
                _ptr = ap._ptr;
                ap._ptr = nullptr;
            }
        }
        T& operator*(){
            return *_ptr;
        }
        T* operator->(){
            return _ptr;
        }
    private:
        T* _ptr;
    };


    //c++11 unique_ptr  防拷贝 （推荐）
    template<typename T>
    class uniquePtr{
    public:
        uniquePtr(T* ptr)
                :_ptr(ptr)
        {}
        //防拷贝就是将智能指针的拷贝构造和重载= 置为删除
        uniquePtr(uniquePtr<T>& p) = delete;
        uniquePtr<T>& operator=(uniquePtr<T>& p) = delete;

        ~uniquePtr(){
            if (_ptr){
                std::cout << "free: "<< _ptr << std::endl;
                delete _ptr;
            }
        }

        T& operator*(){
            return *_ptr;
        }

        T* operator->(){
            return _ptr;
        }

    private:
        T* _ptr;
    };

    //c++11 shared_ptr  共享
    template<typename T>
    class sharedPtr{
    public:
        sharedPtr(T* ptr)
                :_ptr(ptr)
                ,_count(new int(1))
        {}
        sharedPtr(sharedPtr<T>& p)
        :_ptr(p._ptr)
        ,_count(p._count)
        {
            (*_count)++;
        }
        sharedPtr<T>& operator=(sharedPtr<T>& p){
            if (*this != p){
                //如果赋值前，这个智能指针有值，count 不为0 就--count，赋值，如果为0就要先释放旧空间，在赋值
                if((--(*_count)) == 0){
                    delete _ptr;
                    delete _count;
                }
                _ptr = p._ptr;
                _count = p._count;
                (*_count)++;
            }
            return *this;
        }

        ~sharedPtr(){
            if (--(*_count)==0 && _ptr){
                std::cout << "free: "<< _ptr << std::endl;
                delete _ptr;
                _ptr = nullptr;
                delete _count;
                _count = nullptr;
            }
        }

        T& operator*(){
            return *_ptr;
        }

        T* operator->(){
            return _ptr;
        }

        int use_count(){
            return *_count;
        }

    private:
        T* _ptr;
        int* _count;
    };
}

void test_smart_ptr_1();
void test_smart_ptr_2();
void test_smart_ptr_3();
void test_smart_ptr_4();
void test_smart_ptr_5();
void test_smart_ptr_6();