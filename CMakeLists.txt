cmake_minimum_required(VERSION 3.23)
project(CppCode)

set(CMAKE_CXX_STANDARD 14)

include_directories(my_map_and_set)

add_executable(CppCode
        my_map_and_set/my_map.cpp
        my_map_and_set/my_map.h
        my_map_and_set/my_set.cpp
        my_map_and_set/my_set.h
        my_map_and_set/rb_tree.h
        main.cpp
        unordered_map_and_set/unordered_map_and_set.cpp
        unordered_map_and_set/unordered_map_and_set.h
        Hash/HashTable.h
        Hash/my_unordered_map.cpp
        Hash/my_unordered_map.h
        Hash/my_unordered_set.cpp
        Hash/my_unordered_set.h
        Hash/bit_set.cpp
        Hash/bit_set.h
        Hash/BloomFilter.cpp
        Hash/BloomFilter.h
        subject/subject.cpp
        subject/subject.h
        cpp_11/test_cpp_11.cpp
        cpp_11/test_cpp_11.h
        abnormal/abnormal.cpp
        abnormal/abnormal.h
        smartptr/smart_ptr.cpp
        smartptr/smart_ptr.h
        test/test.cpp test/test.h)
