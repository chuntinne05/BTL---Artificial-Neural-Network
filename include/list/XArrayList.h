/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H
#include <memory.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "IList.h"
using namespace std;

template <class T>
class XArrayList : public IList<T>
{
public:
  class Iterator;

protected:
  T *data;
  int capacity;
  int count;
  bool (*itemEqual)(T &lhs, T &rhs);
  void (*deleteUserData)(XArrayList<T> *);

public:
  XArrayList(void (*deleteUserData)(XArrayList<T> *) = 0,
             bool (*itemEqual)(T &, T &) = 0, int capacity = 10);
  XArrayList(const XArrayList<T> &list);
  XArrayList<T> &operator=(const XArrayList<T> &list);
  ~XArrayList();

  // Inherit from IList: BEGIN
  void add(T e);
  void add(int index, T e);
  T removeAt(int index);
  bool removeItem(T item, void (*removeItemData)(T) = 0);
  bool empty();
  int size();
  void clear();
  T &get(int index);
  int indexOf(T item);
  bool contains(T item);
  string toString(string (*item2str)(T &) = 0);
  // Inherit from IList: BEGIN

  void println(string (*item2str)(T &) = 0)
  {
    cout << toString(item2str) << endl;
  }
  void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0)
  {
    this->deleteUserData = deleteUserData;
  }

  Iterator begin() { return Iterator(this, 0); }
  Iterator end() { return Iterator(this, count); }

protected:
  void checkIndex(int index);
  void ensureCapacity(int index);
  void copyFrom(const XArrayList<T> &list);
  void removeInternalData();

  //! FUNTION STATIC
protected:
  static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
  {
    if (itemEqual == 0)
      return lhs == rhs;
    else
      return itemEqual(lhs, rhs);
  }

public:
  static void free(XArrayList<T> *list)
  {
    typename XArrayList<T>::Iterator it = list->begin();
    while (it != list->end())
    {
      delete *it;
      it++;
    }
  }

public:
  class Iterator
  {
  private:
    int cursor;
    XArrayList<T> *pList;

  public:
    Iterator(XArrayList<T> *pList = 0, int index = 0)
    {
      this->pList = pList;
      this->cursor = index;
    }
    Iterator &operator=(const Iterator &iterator)
    {
      cursor = iterator.cursor;
      pList = iterator.pList;
      return *this;
    }
    void remove(void (*removeItemData)(T) = 0)
    {
      T item = pList->removeAt(cursor);
      if (removeItemData != 0)
        removeItemData(item);
      cursor -= 1;
    }

    T &operator*() { return pList->data[cursor]; }
    bool operator!=(const Iterator &iterator)
    {
      return cursor != iterator.cursor;
    }
    Iterator &operator++()
    {
      this->cursor++;
      return *this;
    }
    Iterator operator++(int)
    {
      Iterator iterator = *this;
      ++*this;
      return iterator;
    }
  };
};

//! ////////////////////////////////////////////////////////////////////
//! //////////////////////     METHOD DEFNITION      ///////////////////
//! ////////////////////////////////////////////////////////////////////

template <class T>
XArrayList<T>::XArrayList(
    void (*deleteUserData)(XArrayList<T> *),
    bool (*itemEqual)(T &, T &),
    int capacity)
{
  // TODO
  this->deleteUserData = deleteUserData;
  this->itemEqual = itemEqual;
  this->capacity = capacity;
  this->data = new T[this->capacity];
  this->count = 0;
}

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list)
{
  /*
   * Copies the contents of another XArrayList into this list.
   * Initializes the list with the same capacity as the source list and copies all elements.
   * Also duplicates user-defined comparison and deletion functions, if applicable.
   */
  // TODO
  this->deleteUserData = list.deleteUserData;
  this->itemEqual = list.itemEqual;
  this->capacity = list.capacity;
  this->count = list.count;
  this->data = new T[this->capacity];
  for (int i = 0; i < list.capacity; i++)
  {
    this->data[i] = list.data[i];
  }
}

template <class T>
void XArrayList<T>::removeInternalData()
{
  /*
   * Clears the internal data of the list by deleting the dynamic array and any user-defined data.
   * If a custom deletion function is provided, it is used to free the stored elements.
   * Finally, the dynamic array itself is deallocated from memory.
   */
  // TODO
  // if (this->deleteUserData != nullptr)
  // {
  //   for (int i = 0; i < this->count; i++)
  //   {
  //     this->deleteUserData(this->data[i]);
  //   }
  // }
  // delete[] this->data;
  // this->data = nullptr;
  // this->count = 0;
  // this->capacity = 0;
  if (this->deleteUserData != nullptr)
  {
    this->deleteUserData(this);
  }
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list)
{
  copyFrom(list);
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list)
{
  if (this != &list)
  {
    removeInternalData();
    copyFrom(list);
  }
  return *this;
}

template <class T>
XArrayList<T>::~XArrayList()
{
  removeInternalData();
}

template <class T>
void XArrayList<T>::add(T e)
{
  ensureCapacity(this->count + 1);
  this->data[this->count++] = e;
}

template <class T>
void XArrayList<T>::add(int index, T e)
{
  if (index < 0 || index > this->count)
  {
    throw std::out_of_range("Index is out of range!");
  }
  ensureCapacity(this->count + 1);
  for (int i = this->count; i > index; i--)
  {
    this->data[i] = this->data[i - 1];
  }
  this->data[index] = e;
  this->count++;
}

template <class T>
T XArrayList<T>::removeAt(int index)
{
  checkIndex(index);
  T removedElement = this->data[index];
  for (int i = index; i < this->count - 1; i++)
  {
    this->data[i] = this->data[i + 1];
  }
  this->count--;
  return removedElement;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T))
{
  int idx = indexOf(item);
  if (idx != -1)
  {
    if (removeItemData != nullptr)
    {
      removeItemData(this->data[idx]);
    }
    removeAt(idx);
    return true;
  }
  return false;
}

template <class T>
bool XArrayList<T>::empty()
{
  return this->count == 0;
}

template <class T>
int XArrayList<T>::size()
{
  return this->count;
}

template <class T>
void XArrayList<T>::clear()
{
  // if (this->deleteUserData != nullptr)
  // {
  //   for (int i = 0; i < this->count; ++i)
  //   {
  //     this->deleteUserData(this->data[i]);
  //   }
  // }
  if (this->deleteUserData != nullptr)
  {
    this->deleteUserData(this); // Xóa toàn bộ danh sách, không xóa từng phần tử
  }
  this->count = 0;
}

template <class T>
T &XArrayList<T>::get(int index)
{
  checkIndex(index);
  return this->data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item)
{
  for (int i = 0; i < this->count; ++i)
  {
    if (this->itemEqual != nullptr && this->itemEqual(this->data[i], item))
    {
      return i;
    }
    else if (this->data[i] == item)
    {
      return i;
    }
  }
  return -1;
}
template <class T>
bool XArrayList<T>::contains(T item)
{
  return indexOf(item) != -1;
}

template <class T>
string XArrayList<T>::toString(string (*item2str)(T &))
{
  /**
   * Converts the array list into a string representation, formatting each element using a user-defined function.
   * If no function is provided, it uses the default string representation for each element.
   * Example: Given an array list with elements {1, 2, 3} and a function that converts integers to strings, calling toString would return "[1, 2, 3]".
   *
   * @param item2str A function pointer for converting items of type T to strings. If null, default to the string conversion of T.
   * @return A string representation of the array list with elements separated by commas and enclosed in square brackets.
   */

  // TODO
  // stringstream ss;
  // ss << "[";
  // for (int i = 0; i < this->count; ++i)
  // {
  //   if (i > 0)
  //   {
  //     ss << ", ";
  //   }
  //   ss << (item2str != nullptr ? item2str(this->data[i]) : to_string(this->data[i]));
  // }
  // ss << "]";
  // return ss.str();

  stringstream ss;
  ss << "[";
  for (int i = 0; i < this->count; ++i)
  {
    if (i > 0)
    {
      ss << ", ";
    }

    if (item2str != nullptr)
    {
      ss << item2str(this->data[i]);
    }
    else
    {
      // if constexpr (std::is_pointer<T>::value)
      // {
      //   ss << this->data[i];
      // }
      // else
      // {
      //   ss << to_string(this->data[i]);
      // }
      ss << this->data[i];
    }
  }
  ss << "]";
  return ss.str();
}

//! ////////////////////////////////////////////////////////////////////
//! ////////////////////// (private) METHOD DEFNITION //////////////////
//! ////////////////////////////////////////////////////////////////////
template <class T>
void XArrayList<T>::checkIndex(int index)
{
  /**
   * Validates whether the given index is within the valid range of the list.
   * Throws an std::out_of_range exception if the index is negative or exceeds the number of elements.
   * Ensures safe access to the list's elements by preventing invalid index operations.
   */
  // TODO
  if (index < 0 || index >= this->count)
  {
    throw std::out_of_range("Index is out of range!");
  }
}
template <class T>
void XArrayList<T>::ensureCapacity(int index)
{
  /**
   * Ensures that the list has enough capacity to accommodate the given index.
   * If the index is out of range, it throws an std::out_of_range exception. If the index exceeds the current capacity,
   * reallocates the internal array with increased capacity, copying the existing elements to the new array.
   * In case of memory allocation failure, catches std::bad_alloc.
   */
  // TODO
  if (index > this->capacity)
  {
    int newCapacity = this->capacity * 2;
    T *newArray = new T[newCapacity];
    for (int i = 0; i < this->count; ++i)
    {
      newArray[i] = this->data[i];
    }
    delete[] this->data;
    this->data = newArray;
    this->capacity = newCapacity;
  }
}

#endif /* XARRAYLIST_H */
