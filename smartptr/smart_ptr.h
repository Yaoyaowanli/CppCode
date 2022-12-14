//
// Created by 遥远 on 2022/12/9.
//

#ifndef CPPCODE_SMART_PTR_H
#define CPPCODE_SMART_PTR_H

#endif //CPPCODE_SMART_PTR_H

#pragma once
#include <iostream>
#include <mutex>

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

    template<typename T>
    class weakPtr;

    //c++11 shared_ptr  共享
    // 引用计数，可以拷贝。
    // 缺陷：循环引用
    template<typename T>
    class sharedPtr{
        friend weakPtr<T>;
    public:
        sharedPtr(T* ptr)
                :_ptr(ptr)
                ,_count(new int(1))
                ,_mtx(new std::mutex)
        {}
        sharedPtr(sharedPtr<T>& p)
        :_ptr(p._ptr)
        ,_count(p._count)
        ,_mtx(p._mtx)
        {
            add_ref_count();
        }
        sharedPtr<T>& operator=(sharedPtr<T>& p){
            if (*this != p){
                //如果赋值前，这个智能指针有值，count 不为0 就--count，赋值，如果为0就要先释放旧空间，在赋值
                release();
                _ptr = p._ptr;
                _count = p._count;
                add_ref_count();
            }
            return *this;
        }

        ~sharedPtr(){
            release();
        }

        void add_ref_count(){
            _mtx->lock();
            (*_count)++;
            _mtx->unlock();
        }

        void release(){
            bool flag = false;
            _mtx->lock();
            if (--(*_count) == 0){
                delete _count;
                delete _ptr;
                _count = _ptr = nullptr;
                flag = true;
            }
            if (flag){
                delete _mtx;
                _mtx = nullptr;
                return;
            }
            _mtx->unlock();
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
        std::mutex* _mtx;    //互斥锁指针，因为需要枷锁的是一块空间，多个shared_ptr引用才是安全的
    };

    //c++11 weak_ptr    解决shared——ptr的循环引用问题
    template <typename T>
    class waeakPtr{
    public:
        waeakPtr(const sharedPtr<T>& sp)
        :_ptr(sp._ptr)
        {}
        weakPtr<T>& operator=(const sharedPtr<T>& sp){
            _ptr = sp._ptr;
        }
        T& operator* (){
            return *_ptr;
        }
        T* operator->(){
            return _ptr;
        }

    private:
        T* _ptr;
    };

}

void test_smart_ptr_1();
void test_smart_ptr_2();
void test_smart_ptr_3();
void test_smart_ptr_4();
void test_smart_ptr_5();
void test_smart_ptr_6();
void test_smart_ptr_7();
void test_smart_ptr_8();