//
// Created by 遥远 on 2022/12/4.
//

#include "test_cpp_11.h"
#include <vector>
#include <list>
#include <map>
#include <string>

void test_cpp11_1(){
    //c++11:列表初始化
    int a{10};
    cout << a << endl;
    vector<int> v{1,2,3,4,5};
    for (auto& e:v ){
        cout << e << " ";
    }
    cout << endl;
    list<int> l{5,4,3,2,1};
    for(auto& e:l){
        cout << e << "->" ;
    }
    cout << "nullptr" << endl;
    map<string,int> m1{{"苹果",1},{"香蕉",2},{"西瓜",3}};

    for (auto& e:m1){
        cout << e.first << " : " << e.second << endl;
    }
    //以上这种{}初始化方式时通过 initializer_list<T> 来支持的
}

void test_cpp11_2(){
    //decltype 类型推演     即 RTTI(运行时类型识别) : run time type identification
    int a = 10;
    double b = 20;
    auto c = a+b;
    cout << typeid(c).name() << endl;
    //扩展： typeid 是 typeinfo 下的type_info类，这个类存储了类型信息
    string s;
    //cout << typeid(s).name() << endl;
    //假设想定义一个和c类型一样的对象d，就需要使用decltype
    decltype(c) d;
    //type_info 重载了 ==
    cout << "typeid(d) == typeid(c) ? : " << (typeid(d) == typeid(c)) << endl;

}

//auto 和    范围for

//final 和 override
//  final 用来修饰类，被修饰的类就变成了最终类，他不能被继承。
//  final 还可以用来修饰虚函数，这个虚函数就不能被重写。
//  override 是子类重写虚函数，检查是否完成重写，不满足重写的条件就报错。



//新容器
//之前学习c++98时的容器有：string/vector/list/map/set/deque/bitset + 适配器 stack/queue/priority_queue
//      c++11 新容器：array（定长数组）/forward_list（单链表）/unordered_map/unordered_set


//默认成员函数控制

class A{
public:
    A() = default;  //指定让编译器生成一个默认的构造函数
    A(const int& a)
    :_a(a)
    {}
private:
    int _a = 10;
};
void test_cpp11_3(){
    A a; //No matching constructor for initialization of 'A'
    A aa(a); //ok
}

//假设现在要求A的对象不能拷贝和赋值（防拷贝、赋值）
//c++98
class A1{
    A1(const int& a)
    :_a(a){}
private:
    A1(const A1& a1);   //只声明不定义，再私有限定，防止外部声明，来达到防拷贝、构造
    A1& operator= (const A1& a1);
    int _a = 10;
};

//c++11 删除默认成员函数
class A2{
    A2(const int& a):_a(a){}
    A2(const A2& a2) = delete;
    A2& operator=(const A2& a2) = delete;
private:
    int _a;
};



//右值引用和移动语义（重点）
//c++98 就提出了引用的概念，引用就是给一个对象起别名。
//c++98 左值引用
//c++11 右值引用
//不管是左值引用还是右值引用，他们都是给对象起别名，
//不过左值引用主要是给左值起别名，右值引用主要是给右值起别名。
//什么是左值？什么是右值？
//这里的左右不是方向，例如： int x1 = 10; int x2 = x1; 这里的x1是左值，10是右值，x2也是左值
//可以修改的就可以认为是左值，左值通常是变量
//右值通常是常量，表达式或者函数的返回值（临时对象）
void test_cpp11_4(){
    //左值引用
    int a = 10;
    int& b = a;
    //右值引用
    int x = 1, y = 2;
    int&& c = 10;
    int&& d = x+y;
    //左值引用不能引用右值
    //int& a2 = 10;
    //int& b2 = x+y;
    //但是const 修饰的左值引用可以引用右值
    const int& a2 = 10;
    const int& b2 = x+y;
    //右值引用不能引用左值
    //int&& c1 = a;
    //但是右值引用可以引用move后的左值
    int&& c1 = move(a);
}


template<typename T>
void func (const T& a){
    cout << "func (const T& a)" << endl;
}

template<typename T>
void func (const T&& a){
    cout << "func (const T&& a)" << endl;
}

void test_cpp11_5() {
    int x = 10;
    func(x);
    func(99);
    //func (const T& a)
    //func (const T&& a)
}

//c++11又将右值区分为
// 纯右值： 基本类型的常量或临时对象。
// 将亡值： 自定义类型的临时对象。

//应用场景1：
class my_string {
public:
    my_string(const char* str = ""){
        _str = new char[strlen(str)+1];
        strcpy(_str,str);
    }
    my_string(const my_string& s){
        cout << "my_string(const my_string& s) -> 深拷贝! " << endl;
        _str = new char[strlen(s._str)+1];
        strcpy(_str,s._str);
    }
    my_string(my_string&& s)
    :_str(nullptr)
    {
        //传过来的是一个将亡值，反正参数s都要亡了，不如直接swap让我使用你的资源，我就不用做深拷贝了，节省了效率。
        //当参数是左值就会做深拷贝，右值做移动拷贝。
        cout << "my_string(my_string&& s) -> 移动拷贝!(效率高) " << endl;
        swap(_str,s._str);
    }
    my_string& operator= (const my_string& s){
        cout << "my_string& operator(const my_string& s) -> 深拷贝! " << endl;
        if (this != &s){
            char* newstr = new char[strlen(s._str)+1];
            strcpy(s._str,newstr);
            delete []_str;
            _str = newstr;
        }
        return *this;
    }
    my_string& operator= (my_string&& s){
        cout << "my_string& operator= (my_string&& s) -> 移动拷贝! " << endl;
        swap(_str,s._str);
        return *this;
    }
    ~my_string(){
        delete []_str;
    }
private:
    char* _str;
};

my_string f(const char* s){
    my_string tmp(s);
    return tmp;
}

void test_cpp11_6() {
    my_string s1("左值");
    my_string s2(s1);                   //左值s1做参数
    my_string s3(move(f("右值-将亡值")));    //这里编译器做了优化，move（）变为右值做参数

    s2 = f("右值-将亡值");
}
//结论：所有做深拷贝的自定义类，都可以加上两个右值引用的移动拷贝构造和移动operator=，可以减少深拷贝构造带来的性能消耗。

// 应用场景2：
// 在日常写oj或代码时经常可以看到,某个函数要返回vector/list/。。。。等 ，如果没有右值引用带来的移动拷贝构造和移动operator=
//那么就会在返回时发生多次深拷贝的拷贝构造，效率非常低，有了移动拷贝构造和移动operator= 就会大大的提升效率。
//stl容器基本都实现了移动的拷贝构造和重载= （例：vector：     vector (vector&& x)；vector& operator= (vector&& x); ）
class Solution {
public:
    vector<vector<int>> generate(int numRows) {
        vector<vector<int>> vv;
        //。。。
        return vv;
    }
};

//应用场景3：
//  使用右值引用做函数参数。

//  总结：
//  左值引用 ： 解决的是传参过程中和返回值过程中的拷贝。
//  做参数 ：void func(T x)  ->   void func(const T& x)     解决的是参数传递过程中减少拷贝构造
//  做返回值  ： T func()  ->  T& func()     解决的是返回值传递中的拷贝构造
//  ps ： 这里需要注意有限制，如果返回对象出了作用域就不在了就不能返回引用。

//  右值引用 ： 解决的是传参后，push/insert函数内部将对象移动到容器空间上的问题。
//  + 传值返回接受返回值的拷贝
//  做参数 ： void push(T&& x) 解决的push内部不再使用拷贝构造x到容器空间中，而是将x移动构造过去，因为x是右值/将亡值。
//  做返回值 ： T func() 解决的是外面调用接受func返回对象的拷贝，T ret = func() ，这里就是右值引用的移动构造，减少了拷贝。



//c++11 通过forward函数来实现完美转发

void Func(int& x){cout << "left value ref " << endl;}
void Func(int&& x){cout << "right value ref " << endl;}
void Func(const int& x){cout << "const left value ref " << endl;}
void Func(const int&& x){cout << "const right value ref " << endl;}

template<typename T>
void PerfectForward(T && t){
    Func(std::forward<T>(t));
}

void test_cpp11_7(){
    PerfectForward(10); //right value

    int a;
    PerfectForward(a); //left value
    PerfectForward(move(a)); //right value

    const int b = 8;
    PerfectForward(b);  //left value
    PerfectForward(move(b)); //right value
}

//没有进行完美转发时的输出为：
//left value ref
//left value ref
//left value ref
//const left value ref
//const left value ref
// 这里说明右值引用会在第二次及以后的参数传递过程中丢失右值属性

//在第二次参数传递处，加上完美转发函数 Func(std::forward<T>(t)); 后输出结果为：
//right value ref
//left value ref
//right value ref
//const left value ref
//const right value ref
//这时就正确了


//  lambda 表达式
//  lambda表达式是一种匿名函数
void test_cpp11_8(){
    //定义在函数中的匿名函数
    int a = 10;
    int b = 20;
    //实现a+b的lambda表达式
    //不捕捉
    auto add1 = [](int x1,int x2)->int{return x1+x2;};
    cout << add1(a,b) << endl;
    //直接捕捉a，b
    auto add2 = [a,b]()->int {return a+b;};
    cout << add2() << endl;

}
