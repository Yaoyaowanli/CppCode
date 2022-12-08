//
// Created by 遥远 on 2022/12/8.
//

#include "abnormal.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;


//什么是异常？
// 异常是面向对象语法处理错误的一种方式。

//c语言传统的错误处理方式有哪些？
// 1、返回错误码  有些API接口都是把错误码放到一个errno中。
// 2、终止程序    比如发生越界等严重错误时，可以调用exit(-1) 或者assert() 来终止程序

//传统错误处理的缺陷：
//如果一个函数是通过返回值拿数据，发生错误就很难处理。
//如果调用的函数栈很深，就需要一层层的返回错误码，很麻烦

//c++ 异常
void test_abnormal_1(){
    vector<int> v {1,2,3,4,5};
    try {   //有可能发生错误的地方需要try
        for (int i=0; i<= v.size(); i++){
            cout << v.at(i) << " " ;    //[]以assert断言终止。at或抛异常
        }
        cout << endl;
    } catch (exception& e) {    //catch 是捕获错误
        cout << e.what() << endl;
    }
}

namespace yao{
    int div(int x,int y){
        if ( y==0){
            throw string("error: 除0！");     //如果有异常，会直接跳到catch匹配的地方
        }
        return x/y;
    }
}

void test_abnormal_2() {
    int m,n;
    cin >> m >> n ;
    try {
        cout << yao::div(m,n) << endl;
    } catch (string& s) {
        cout<< __LINE__ << s << endl;
    }

    catch (...){    //catch(...)可以捕获没有匹配的任何类型的异常,避免有异常没捕获程序终止
        cout << "未知异常" << endl;
    }
}


//自定义异常体系
// 在实际中，并不是我们像抛什么异常就抛什么异常，这样会导致捕捉时不好捕捉，而是会建立一个继承体系，建立一个异常类，派生类继承该异常类，
// 来定义出不同的异常，到时候抛出异常，只需要用基类捕捉就可以了


//可以抛自己定义的异常，但是必须继承这个基类，这样的话，外层只用捕获基类就可以
class Exception{
public:
    Exception(const char* errmsg,const int errId)   //派生类构造必须调用基类构造函数构造基类部分。
    :_errId(errId)
    ,_errmsg(errmsg)
    {}
    virtual string what() = 0;
protected:
    int _errId;     //错误码
    string _errmsg; //错误信息
    //stack<string> _st; //调用的函数栈帧
};

class SqlException : public Exception{
    //....
public:
    SqlException(const char* errmsg,const int errId)
    : _sql_errmsg(errmsg)
    , Exception(errmsg,errId)
    {}
    virtual string what() override {
         return "this is sql error: "+_errmsg ;
    }
private:
    string _sql_errmsg;
};

class NetException : public Exception{
    //...
public:
    NetException(const char* errmsg,const int errId)
    :_net_errmsg(errmsg)
    , Exception(errmsg,errId)
    {}
    virtual string what() override {
        return "this is net error: " + _errmsg;
    }
private:
    string _net_errmsg;
};


void func (){
    //....
    if (rand() % 3 ==0 ){
        //假设执行失败了
        throw SqlException("sql 错误：...",1);
    }
    if (rand() % 7 ==0){
        throw NetException("net 错误：...",3);
    }
    cout << "正常～" << endl;
}

void test_abnormal_3() {
    int i = 100;
    while (i--){
        try{
            func();
        } catch (Exception& e) {
            cout << e.what() << endl;
        } catch (...) {
            cout << "未知" << endl;
        }
    }
}


//异常可能会导致异常安全问题
// new/fopen/lock

// func（）； //如果在这个函数中抛了异常，就可能导致打开的文件句柄/开辟的空间，得不到关闭/释放，造成资源泄漏
                                //解决方式： 1、先捕获之后释放资源，再重新抛出异常。2、RAII
// delete/fclose/unlock

//函数规范一下，如果要抛异常要说明清楚，不抛也要说明一下


//这里表示可能会抛上面的三种类型的异常
void fun() throw(Exception,SqlException,NetException);
//这里表示只会抛出std::bad_alloc的异常
void* operator new(std::size_t size) throw(std::bad_alloc);
//这里表示不会抛出异常
void* operator new(std::size_t size,void* ptr) throw();
//不会抛出异常
void* operator new(std::size_t size,void* ptr) noexcept;


//ps ： 构造函数和析构函数最好不要抛异常！

//总结：异常的优缺点
//优点：
//  1、可以清晰的包含错误信息。  2、面对 T& operator[] ()这样的函数如果越界，异常可以很好的解决问题。3、多层调用时里面发生错误，
//  不再需要层层处理，最外层直接处理。  4、很多第三方库都使用异常，我们也使用可以更好的配合处理。

//缺点：
//  1、异常会导致执行流比较乱，会让我们分析bug调试程序变得困难。
//  2、异常导致资源泄漏等异常安全问题，需要使用RAII解决。
//  3、c++库的异常体系定义的不好用
//  4、c++的异常可以抛任意类型的异常，如果项目中没有很好的规范管理，那么会非常混乱。所以一般需要定义继承体系的异常规范。

//异常整体而言还是利大于弊的。