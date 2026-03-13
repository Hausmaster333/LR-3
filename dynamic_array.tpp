#include "dynamic_array.h"
#include <stdexcept>

template <class T>
DynamicArray<T>::DynamicArray() : size(0) {
    data = new T[4];
}

template <class T>
DynamicArray<T>::DynamicArray(int size): size(size) {
    data = new T[size];
}

template <class T>
DynamicArray<T>::DynamicArray(const T* items, int count): size(count) {
    data = new T[size];

    for (int i = 0; i < size; i++) {
        data[i] = items[i];
    }
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other): size(other.size) {
    data = new T[size];

    for (int i = 0; i < size; i++) {
        data[i] = other.data[i];
    }
}

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this == &other) return *this; // Чтобы не самоприсваивать

    delete[] data;
    size = other.size;
    data = new T[size];

    for (int i = 0; i < size; i++) {
        data[i] = other.data[i];
    }

    return *this;
}

template <class T>
const T& DynamicArray<T>::get(int index) const {
    if (index >= size || index < 0) {
        throw std::out_of_range("Index out of range");
    }

    return data[index];
}

template <class T>
int DynamicArray<T>::get_size() const {
    return size;
}

template <class T>
void DynamicArray<T>::set(int index, const T& value) {
    if (index >= size || index < 0) {
        throw std::out_of_range("Index out of range");
    }
    data[index] = value;
}

template <class T>
void DynamicArray<T>::resize(int newSize) {
    T* newData = new T[newSize];

    int mainSize = (newSize < size) ?  newSize : size;

    for (int i = 0; i < mainSize; i++) {
        newData[i] = data[i];
    }

    delete[] data;
    data = newData;
    size = newSize;
}

template <class T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}
