//
// Created by 遥远 on 2022/12/4.
//

#ifndef CPPCODE_TEST_CPP_11_H
#define CPPCODE_TEST_CPP_11_H

#endif //CPPCODE_TEST_CPP_11_H

#pragma once
#include <iostream>
using namespace std;



/*
class Person  {
public:
    virtual  void print() final {
        cout << _name << endl;
    }
private:
    string _name;
    int _id;
};

class yao : public Person{
    void print(){}; //error : Declaration of 'print' overrides a 'final' function
};
*/

/*
class Base{
public:
    virtual void print_n(){
        cout << "base" << _n << endl;
    }
private:
    int _n;
};

class Son : public Base{
public:
    void print_n() override {

    }
private:
    int _son_n;
};
*/



void test_cpp11_1();
void test_cpp11_2();
void test_cpp11_5();
void test_cpp11_6();
void test_cpp11_7();
void test_cpp11_8();
void test_lambda_1();
void test_lambda_2();
void test_lambda_3();
void test_lambda_4();
void test_thread_1();
void test_thread_2();
