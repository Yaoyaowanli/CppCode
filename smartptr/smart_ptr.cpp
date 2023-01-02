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



//单例模式
//  设计一个类，要求该类智能在堆上创建对象

//思路：正常对象创建，一定要调用类的构造函数和拷贝构造函数
class HeapOnly{
public:
     static HeapOnly* GetObj(){
        return new HeapOnly;
    }
private:
    HeapOnly(){}
    HeapOnly(const HeapOnly& h) = delete;

};

//设计一个类只能在栈上创建
class StackOnly{
public:
    void* operator new(size_t size) = delete;   //不建议

    static StackOnly getObj(){
        return StackOnly();
    }
private:
    StackOnly(){}
};

/*void test_1() {
    //没有对象还是无法调用GetObj。解决办法，将GetObj设为static修饰的
    HeapOnly* p = HeapOnly::GetObj();
    StackOnly p2 = StackOnly::getObj();
}*/

//单例模式： 一个类在全局、进程中只能有一个实例对象，在使用的时候再创建对象
// 应用场景： 内存池，进程中的多线程需要内存都去内存池取，这个内存池类，就可以设计为单例
//懒汉模式
class Memory_pool{
public:
    static Memory_pool* get_pool(){
        //_mtx->lock();   //确保线程安全

        //这个域用来确定unique_lock 的作用范围，出了该域unique_lock 就会解锁。
        //但是我们只需要初始化一次，后续每次都要加锁太浪费性能了
        if (_pMemory == nullptr)    //双检查，第一次初始化的时候需要进去加锁new对象，后续就不需要加锁浪费资源了。
            //如果不再加一次判断，以后每次走到这个域都会多出一个锁的资源，还会阻塞其他线程
        {//使用unique_lock 来确保抛异常后还可以解锁
            unique_lock<mutex> lock(*_mtx);

            if (_pMemory == nullptr) {
                //new有可能抛异常，一旦抛异常就没法解锁了，其余线程就死锁了。
                _pMemory = new Memory_pool;
            }
        }

        //_mtx->unlock();
        return _pMemory;
    }
private:
    Memory_pool(){}
    Memory_pool(const Memory_pool& mp) = delete;

    static Memory_pool* _pMemory;
    static mutex* _mtx;
};

Memory_pool* Memory_pool::_pMemory = nullptr;
mutex* Memory_pool::_mtx = new mutex;

//总结：Memory_pool 全局只有一个，所以私有化构造函数，删除了拷贝构造函数，私有成员pMemory和mutex都是static的，该类共用一个全局成员。
//      获得对象只有通过get_pool接口来进行，而该接口在第一次调用的时候
//      会返回一个memory_pool对象指针，之后返回的都是同一个对象的指针，又加入了互斥锁来保证线程安全问题，多个线程第一次创建pool对象
//      时不会发生竞争，unique_lock防止了new的过程中如果抛异常，线程会死锁的问题。


//饿汉模式： main函数之前就创建对象
class SingLeton{
public:
    static SingLeton* Get_SingObj(){
        return &_inst;
    }

private:
    static SingLeton _inst;
};

SingLeton SingLeton::_inst;         //static 是在main函数之前创建的，没有多线程，不需要在意线程安全





//c++ 类型转换

void test_cast_1(){
    //c++ 兼容c语言留下来的隐式转换和显示转换。
    //标准c++为了加强类型转换的可视性，引入了4种命名的强制类型转换操作符
    // static_cast    reinterpret_cast   const_cast   dynamic_cast
    int i = 1;
    double d = 9.99;
    i = d;  //隐士类型转化，丢失精度

    d = static_cast<double>(i);     //对应c的隐士类型转换
    int* p = nullptr;
    p = reinterpret_cast<int*>(i);  //对应c的强制类型转换（不相近类型）

    volatile const int ci = 99;
    int* pi = const_cast<int*>(&ci);    //const_cast 去除const属性
    *pi = 11;
    cout << *pi  << ": " << pi << endl;     //11: 0x16f5ef514
    //cout << ci <<  ": " << &ci << endl;     //99: 0x16f5ef514 （这里的99是编译器优化了直接去寄存器拿的，其实内存还是被修改了）
    // 为什么地址相同但是打印出得数值却不同呢？ pi指向了ci的空间，通过const_cast消除了const修改了
    //空间的值为11 我们通过调试看到这块空间确实变为11了，那为什么打印的ci是99呢？ 因为ci开始是const修饰的会放到寄存器中，而调试是去内存中
    //取得值，所以会不一样，打印是去寄存器拿到的ci,这是被编译器优化过的，要想取消去寄存器取值的优化，就要在变量前加volatile


    //dynamic_cast
    //      基类对象指针 = &子类对象 （可以赋，会发生切割）  子类对象指针 = &基类对象（错误）
    //      子类对象指针 = （子类对象指针）基类对象    c语言强制转换可以成功，但是访问子类的成员时会发生越界
    //      子类对象指针 =  （子类对象指针）原本指向子类对象的基类对象  成功
    //      这个时候我们就可以使用dynamic_cast<转换对象类型>(对象/指针) 如果该对象/指针原本是指向子类的会成功返回子类对象指针
    //      如果原本不是指向子类对象的，则会返回nullptr
    //      ps：只能用于多态类型
}