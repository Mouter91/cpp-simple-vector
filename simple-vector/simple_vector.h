#pragma once

#include <cassert>
#include <initializer_list>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include "array_ptr.h"

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity) : capacity_res(capacity) {}
    size_t GetCapacity() const {
        return capacity_res;
    }
private:
    size_t capacity_res;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
        : items(new Type[size]), size_(size), capacity_(size) {
        std::fill(items.get(), items.get() + size_, Type());
    }

    SimpleVector(size_t size, const Type& value)
        : items(new Type[size]), size_(size), capacity_(size) {
        std::fill(items.get(), items.get() + size_, value);
    }

    SimpleVector(std::initializer_list<Type> init)
        : items(new Type[init.size()]), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), items.get());
    }

    SimpleVector(const SimpleVector& other)
        : items(new Type[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
        std::copy(other.items.get(), other.items.get() + size_, items.get());
    }

    SimpleVector(SimpleVector&& other) noexcept
        : items(std::move(other.items)), size_(other.size_), capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    explicit SimpleVector(const ReserveProxyObj& proxy)
        : items(new Type[proxy.GetCapacity()]), size_(0), capacity_(proxy.GetCapacity()) {}

    SimpleVector& operator=(const SimpleVector& other) {
        if (this != &other) {
            SimpleVector temp(other);
            swap(temp);
        }
        return *this;
    }

    void PopBack() noexcept {
        if (size_ > 0) {
            --size_;
        }
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            array_ptr<Type> new_items(new Type[new_capacity]);

            std::move(items.get(), items.get() + size_, new_items.get());

            items = std::move(new_items);
            capacity_ = new_capacity;
        }
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(items, other.items);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }


    void PushBack(Type&& item) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            array_ptr<Type> new_items(new Type[new_capacity]);
            std::move(items.get(), items.get() + size_, new_items.get());
            items = std::move(new_items);
            capacity_ = new_capacity;
        }
        items[size_++] = std::move(item);
    }


    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t index = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            array_ptr<Type> new_items(new Type[new_capacity]);
            std::move(items.get(), items.get() + index, new_items.get());
            std::move(items.get() + index, items.get() + size_, new_items.get() + index + 1);
            items = std::move(new_items);
            capacity_ = new_capacity;
        } else {
            std::move_backward(begin() + index, end(), end() + 1);
        }
        items[index] = std::move(value);
        ++size_;
        return begin() + index;
    }

    Iterator Erase(ConstIterator pos) {
        size_t index = pos - begin();
        std::move(begin() + index + 1, end(), begin() + index);
        --size_;
        return begin() + index;
    }


    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        return items[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return items[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return items[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return items[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            size_t new_capacity = new_size * 2;
            array_ptr<Type> new_items(new Type[new_capacity]);

            std::move(items.get(), items.get() + size_, new_items.get());

            for (size_t i = size_; i < new_size; ++i) {
                new_items[i] = Type();
            }

            items = std::move(new_items);
            capacity_ = new_capacity;
        } else if (new_size > size_) {

            for (size_t i = size_; i < new_size; ++i) {
                items[i] = Type();
            }
        }
        // Обновляем размер
        size_ = new_size;
    }


    Iterator begin() noexcept {
        return items.get();
    }

    Iterator end() noexcept {
        return items.get() + size_;
    }

    ConstIterator begin() const noexcept {
        return items.get();
    }

    ConstIterator end() const noexcept {
        return items.get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return items.get();
    }

    ConstIterator cend() const noexcept {
        return items.get() + size_;
    }

private:
    array_ptr<Type> items; в
    size_t size_{0};
    size_t capacity_{0};
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) return false;
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
