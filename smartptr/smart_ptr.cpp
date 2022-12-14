//
// Created by 遥远 on 2022/12/9.
//
//  智能指针
#include <thread>
#include <memory>
#include "smart_ptr.h"
using namespace std;



namespace yao{
    int div() throw(invalid_argument){
        int x,y;
        cin >> x >> y ;
        if (y == 0){
            throw invalid_argument("除0错误!");
        }
        return x/y;
    }
}

/*void func1(){
    int *p = new int;

    int ret = yao::div();
    //div函数里面会抛异常，我们在test_smart_ptr_1函数捕获异常，所以这里的资源p需要想办法在有异常的情况下释放掉。
    delete p;
}*/

/*void func1(){
    int* p = new int ;
    try{
        int ret = yao::div();
    } catch (invalid_argument& e) {
        delete p;       //如果有异常，我们先进行捕获，然后释放资源，在将该异常抛出。
        throw e;
    }
    delete p;
}*/
void func1(){
    yao::SmartPtr<int> ps(new int);  //智能指针会在声明周期结束时自动析构，不管是不是抛了异常，只要他的生命周期结束，就会析构。
    int ret = yao::div();
}
void test_smart_ptr_1(){
    try{
        func1();
    }catch (exception& e){
        cout << e.what() << endl;
    }
}


//智能指针的坑
void test_smart_ptr_2() {
    yao::SmartPtr<int> ps1(new int );
    yao::SmartPtr<int> ps2 = ps1;    //这里就会报错，因为ps1结束时释放了空间，ps2又去释放了一次
    // 我们需要让智能指针像原生指针一样，一个智能指针在赋值给别的智能指针之后，不管赋值或者拷贝构造了多少个指向同一块空间的智能指针
    // 这块空间都只能被释放一次。
}

//有三种解决方法：  1、管理权转移     2、防拷贝   3、引用计数
// c++98 auto_ptr 管理权转移
//缺陷是 p2 = p1 后p1就会将自己置空，如果不知道的情况下*p1，就会崩溃
void test_smart_ptr_3(){
    yao::auto_ptr<int> p1(new int );
    yao::auto_ptr<int> p2 = p1;     // p2接管了空间，p1将自己置空了，此时p1就是一个空指针
}
// c++11 unique_ptr 防拷贝
// 缺陷：不能拷贝
void test_smart_ptr_4(){
    yao::uniquePtr<int> p1(new int);
    //auto p2(p1);  调用已删除的'uniquePtr'的构造函数
    //auto p3 = p1;
}
// c++11 shared_ptr 引用计数的共享拷贝
//          循环引用的问题，又需要weak_ptr解决
void test_smart_ptr_5(){
    yao::sharedPtr<int> p1(new int);
    *p1 = 99;
    yao::sharedPtr<int> p2 = p1;
    yao::sharedPtr<int> p3(p1);
    yao::sharedPtr<int> sp1(new int);
    auto sp2 = sp1;
}



//关于shared_ptr的线程安全问题
void test_smart_ptr_6() {
    yao::sharedPtr<int> p(new int);
    cout << p.use_count() << endl;
    int n = 10000;

    std::thread t1([&]()->void{
        for (int i=0;i<n;i++){
            yao::sharedPtr<int> p2 = p;
        }
    });
    std::thread t2([&]()->void{
        for (int i=0;i<n;i++){
            yao::sharedPtr<int> p2 = p;
        }
    });
    //这里t1、t2两个线程同时取拿对象拷贝构造p，由于p中count的++、--操作不是原子操作，汇编有三步（放入寄存器，++，放入变量）
    //所以可能在_count = 1 时同时两个线程拿到这个值执行，结果本来是3的却成了2。
    t1.join();
    t2.join();

    cout << p.use_count() << endl;
}

/*
//关于shared_ptr 的循环引用问题
struct sharedPtrListNode{
    ~sharedPtrListNode(){
        cout << " ~sharedPtrList() " << endl;
    }
private:
    int _val;
    yao::sharedPtr<int> _prev;
    yao::sharedPtr<int> _next;
};
void test_smart_ptr_7() {
    sharedPtrListNode p1 (new sharedPtrListNode);

}*/

//定制删除器
template<typename T>
struct delete_array{
    void operator()(T* pa){
        delete[] pa;
    }
};

struct fclose_file{
    void operator()(FILE* f){
        cout << "释放文件句柄～" << endl;
        fclose(f);
    }
};

class A{
public:
    ~A(){
        cout << "~A" << endl;
    }
private:
    int _a1;
    int _a2;
};
void test_smart_ptr_7() {
    std::shared_ptr<A> sp1(new A);  //这里的sp1 在函数结束后智能指针释放了A
    //std::shared_ptr<A> sp2(new A[10]);// 但是sp2 奔溃了，那么对于这种数组类型，我们可以使用定制删除器的方式去定义如何释放他们。
    //我们定制了一个彷函数，重载了（） 在函数里定义了array版本使用delete[]来删除数组，然后将彷函数对象传给shared_ptr
    std::shared_ptr<A> sp2(new A[10],delete_array<A>());
    //再比方说我们获得了一个文件资源，文件句柄的释放不是使用delete，那么我们也可以利用彷函数定制释放资源的方式来改变shared_ptr的释放方式
    std::shared_ptr<FILE> sp3(fopen("./test.txt","w"),fclose_file());
}


//智能指针是RAII思想的一种应用的体现
//本质上RAII就是借助类的构造和析构函数的自动调用特性来释放资源

//使用RAII 思想设计互斥锁的管理首位
template<typename lock>
class lockGuard{
public:
    explicit lockGuard(lock& lk)
    :_lk(lk)
    {
        cout << "lock()" << endl;
        lk.lock();
    }

    ~lockGuard(){
        cout << "unlock()" << endl;
        _lk.unlock();
    }
private:
    lock& _lk;
};


void func2(){
    mutex mtx;
    lockGuard<mutex> lg(mtx);   //
    //mtx.lock();
    yao::div();
    //........      假设中间代码可能会抛异常
    //mtx.unlock();
}

void test_smart_ptr_8() {
    try{
        thread t1(func2);    //如果现在t1线程在执行中间代码时发生了异常，没有释放互斥锁，那么t2线程就会一直阻塞，导致死锁
        thread t2(func2);
        t1.join();
        t2.join();
    }catch (invalid_argument& e){
        cout << e.what() << endl;
    }

}

