#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <iostream>
#include <stdexcept>
#include <utility>

template <typename T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;

    void resize() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < size; ++i) {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    DynamicArray(int initialCapacity = 4) : size(0), capacity(initialCapacity) {
        if (capacity <= 0) capacity = 4;
        data = new T[capacity];
    }

    ~DynamicArray() {
        delete[] data;
    }

    // Добавляем конструктор копирования
    DynamicArray(const DynamicArray& other) : size(other.size), capacity(other.capacity) {
        data = new T[capacity];
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }

    // Добавляем оператор присваивания
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            capacity = other.capacity;
            data = new T[capacity];
            for (int i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    DynamicArray(DynamicArray&& other) noexcept 
        : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            capacity = other.capacity;
            other.data = nullptr;
            other.size = 0;
            other.capacity = 0;
        }
        return *this;
    }

    void push_back(T value) {
        if (size >= capacity) {
            resize();
        }
        data[size++] = std::move(value);
    }
    
    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (size >= capacity) {
            resize();
        }
        data[size++] = T(std::forward<Args>(args)...);
    }

    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }

    int get_size() const { return size; }
    
    void clear() {
        delete[] data;
        size = 0;
        capacity = 4;
        data = new T[capacity];
    }
};

#endif
