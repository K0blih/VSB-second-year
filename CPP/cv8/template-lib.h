#pragma once

#include <vector>

int foo1(int x);

// This will not compile - why?
// What is the difference between foo1 and foo2?
template <typename T>
int foo2(T t);

/* Solutions:
 * 1) Include the implementation file in each translation unit or write implementation into header file
 * - most commonly used
 * 2) Create an explicit template instantiation
 * */

/*
 * Template functions will be inline and compiled in each translation unit -> can be wasteful.
 * You can use extern template to avoid instantiation a templated function/class in a specific TU (https://stackoverflow.com/questions/8130602/using-extern-template-c11).
 */

