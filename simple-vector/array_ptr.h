#pragma once

#include <cstddef>
#include <utility>

template <typename Type>
class ArrayPtr {
public:

    explicit ArrayPtr(Type* ptr = nullptr) noexcept : ptr_(ptr) {}
    ArrayPtr(size_t size) noexcept : ptr_(new Type[size]) {}

    ArrayPtr(ArrayPtr& other) = delete;
    ArrayPtr& operator=(ArrayPtr& other) = delete;

    ArrayPtr(ArrayPtr&& other) : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    ArrayPtr& operator=(ArrayPtr&& other) {
        if(this != &other) {
            delete [] ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    ~ArrayPtr() {
        delete [] ptr_;
    }

    Type& operator[](std::size_t index) noexcept {
        return ptr_[index];
    }

    const Type& operator[](std::size_t index) const noexcept {
        return ptr_[index];
    }

    Type* get() noexcept {
        return ptr_;
    }

    const Type* get() const noexcept {
        return ptr_;
    }

private:
    Type* ptr_;
};
