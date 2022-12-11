//
// Created by 遥远 on 2022/12/9.
//
//  智能指针
#include <thread>
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

    t1.join();
    t2.join();

    cout << p.use_count() << endl;
}