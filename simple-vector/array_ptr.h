#pragma once

#include <cstddef>
#include <utility>

template <typename Type>
class array_ptr {
public:

    explicit array_ptr(Type* ptr = nullptr) noexcept : ptr_(ptr) {}

    array_ptr(array_ptr& other) = delete;
    array_ptr& operator=(array_ptr& other) = delete;

    array_ptr(array_ptr&& other) : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    array_ptr& operator=(array_ptr&& other) {
        if(this != &other) {
            delete [] ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    ~array_ptr() {
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
