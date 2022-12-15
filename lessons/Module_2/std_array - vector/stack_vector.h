#pragma once

#include <array>
#include <stdexcept>
#include <iostream>

template <typename T, size_t N>
class StackVector {
public:
    explicit StackVector(size_t a_size = 0) {
        if (a_size <= N) {
            a_size_ = a_size;
        } else {
            throw std::invalid_argument("Error: Size of the vector bigger than its capacity");
        }
    }

    T& operator[](size_t index) {
        if (index < a_size_) {
            return _vector[index];
        } else {
            throw std::invalid_argument("Error: Wrong index");
        }

    }
    const T& operator[](size_t index) const {
        if (index < a_size_) {
            return _vector.at(index);
        } else {
            throw std::invalid_argument("Error: Wrong index");
        }
    }

    typename std::array<T, N>::iterator begin() {
        return _vector.begin();
    }
    typename std::array<T, N>::iterator end() {
        return _vector.begin() + a_size_;
    }
    
    typename std::array<T, N>::const_iterator begin() const {
        return _vector.begin();
    }

    typename std::array<T, N>::const_iterator end() const {
        return _vector.begin() + a_size_;
    }

    size_t Size() const {
        return a_size_;
    }
    size_t Capacity() const {
        return _vector.max_size();
    }

    void PushBack(const T& value) {
        if (Size() < Capacity()){
            _vector[Size()] = value;
            ++a_size_;
        } else {
            throw std::overflow_error("Error: Vector is full");
        }
    }

    T PopBack() {
        if (Size() > 0) {
            T temp = _vector[Size() - 1];
            --a_size_;
            return temp;
        } else {
            throw std::underflow_error("Error: Vector is empty");
        }
    }
private:
    std::array<T, N> _vector;
    size_t a_size_ = 0;
};
