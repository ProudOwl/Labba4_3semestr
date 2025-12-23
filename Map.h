#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <stdexcept>

template <typename K, typename V>
class Map {
public:
    struct Pair {
        K key;
        V value;
        Pair() {}
        Pair(const K& k, const V& v) : key(k), value(v) {}
    };

private:
    Pair* data;
    size_t capacity;
    size_t length;

    void resize() {
        size_t newCapacity = (capacity == 0) ? 4 : capacity * 2;
        Pair* newData = new Pair[newCapacity];
        for (size_t i = 0; i < length; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    Map() : data(nullptr), capacity(0), length(0) {}

    Map(const Map& other) : capacity(other.capacity), length(other.length) {
        data = new Pair[capacity];
        for (size_t i = 0; i < length; ++i) {
            data[i] = other.data[i];
        }
    }
    
    Map(Map&& other) noexcept : data(other.data), capacity(other.capacity), length(other.length) {
        other.data = nullptr;
        other.length = 0;
        other.capacity = 0;
    }

    ~Map() { delete[] data; }

    Map& operator=(const Map& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            length = other.length;
            data = new Pair[capacity];
            for (size_t i = 0; i < length; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    void insert(const K& key, const V& value) {
        if (find(key)) {
            (*this)[key] = value;
            return;
        }
        if (length >= capacity) resize();
        data[length++] = Pair(key, value);
    }

    bool find(const K& key) const {
        for (size_t i = 0; i < length; ++i) {
            if (data[i].key == key) return true;
        }
        return false;
    }

    V& operator[](const K& key) {
        for (size_t i = 0; i < length; ++i) {
            if (data[i].key == key) return data[i].value;
        }
        if (length >= capacity) resize();
        data[length] = Pair(key, V());
        return data[length++].value;
    }

    const V& at(const K& key) const {
        for (size_t i = 0; i < length; ++i) {
            if (data[i].key == key) return data[i].value;
        }
        throw std::out_of_range("Key not found");
    }
    
    bool count(const K& key) const { return find(key); }

    size_t size() const { return length; }
    bool empty() const { return length == 0; }
    void clear() {
        delete[] data;
        data = nullptr;
        length = 0;
        capacity = 0;
    }

    class Iterator {
    private:
        Pair* ptr;
    public:
        Iterator(Pair* p) : ptr(p) {}
        Pair& operator*() { return *ptr; }
        const Pair& operator*() const { return *ptr; }
        Pair* operator->() { return ptr; }
        const Pair* operator->() const { return ptr; }
        Iterator& operator++() { ++ptr; return *this; }
        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
    };

    Iterator begin() { return Iterator(data); }
    Iterator end() { return Iterator(data + length); }
    Iterator begin() const { return Iterator(data); }
    Iterator end() const { return Iterator(data + length); }
};

#endif
