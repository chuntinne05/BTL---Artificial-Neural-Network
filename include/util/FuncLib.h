#ifndef FUNCLIB_H
#define FUNCLIB_H

#include <iostream>
#include <stdexcept>

using namespace std;

// Swap function for two elements
template <class T>
void swap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

// Compare two elements (ascending order)
template <class T>
int compareAsc(T &lhs, T &rhs)
{
    if (lhs < rhs)
        return -1;
    if (lhs > rhs)
        return 1;
    return 0;
}

// Compare two elements (descending order)
template <class T>
int compareDesc(T &lhs, T &rhs)
{
    if (lhs < rhs)
        return 1;
    if (lhs > rhs)
        return -1;
    return 0;
}

// Free memory if T is pointer
template <class T>
void free(T *ptr)
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

// Throw underflow exception if heap is empty
void checkEmpty(bool isEmpty)
{
    if (isEmpty)
    {
        throw std::underflow_error("Calling to peek with the empty heap.");
    }
}

#endif /* FUNCLIB_H */
