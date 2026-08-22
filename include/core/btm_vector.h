#pragma once

#include <stdexcept>

namespace btm {
    // custom vector implementation for fast push_back and access, without using std::vector
    // it does not support shrinking, only growing when capacity is reached
    // it is not as full featured as std::vector, but it is faster for our use case
    template <typename T>
    class btm_vector {
        T* data;
        std::size_t d_size;
        std::size_t current;

        inline bool capacity_reached() const {
            return current >= d_size;
        }
        inline void grow(std::size_t new_size) {
            if (new_size <= d_size) return; // No need to shrink or keep the same size
            T* new_data = new T[new_size];
            std::copy(data, data + current, new_data);
            delete[] data;
            data = new_data;
            d_size = new_size;
        }
    public:
        btm_vector(std::size_t max_size = 16) : d_size(max_size), current(0) {
            data = new T[d_size];
        }
        btm_vector(const btm_vector& other) {
            d_size = other.d_size;
            data = new T[d_size];
            current = other.current;
            std::copy(other.data, other.data + current, data);
        }
        btm_vector(std::initializer_list<T> init) : d_size(init.size()), current(init.size()) {
            data = new T[d_size];
            std::copy(init.begin(), init.end(), data);
        }
        ~btm_vector() {
            delete[] data;
        }
        inline void push_back(const T& value) {
            if (capacity_reached()) {
                grow(d_size * 2); // Double the size if capacity is reached
            }
            data[current++] = value;
        }
        inline T operator[](std::size_t index) const {
            if (index >= current) throw std::out_of_range("Index out of range");
            return data[index];
        }
        inline T& operator[](std::size_t index) {
            if (index >= current) throw std::out_of_range("Index out of range");
            return data[index];
        }
        inline size_t size() const {
            return current;
        }
        inline bool empty() const {
            return current == 0;
        }
        inline void clear() {
            current = 0;
        }
        inline void resize(size_t new_size) {
            if (new_size > d_size) {
                T* new_data = new T[new_size];
                data = new_data;
                d_size = new_size;
            }
            current = new_size;
        }
        inline void reserve(size_t new_capacity) {
            if (new_capacity > d_size) {
                grow(new_capacity);
            }
            current = 0; // Reset current to 0 after reserving new capacity
        }
        inline btm_vector& operator=(const btm_vector& other) {
            if (this != &other) {
                if (d_size < other.d_size) {
                    delete[] data;
                    d_size = other.d_size;
                    data = new T[d_size];
                }
                current = other.current;
                std::copy(other.data, other.data + current, data);
            }
            return *this;
        }
        inline T* begin() {
            return data;
        }
        inline const T* begin() const {
            return data;
        }
        inline T* end() {
            return data + current;
        }
        inline const T* end() const {
            return data + current;
        }
        inline T* erase(T* pos_start, T* pos_end) {
            if (pos_start < data || pos_end > data + current || pos_start > pos_end) {
                throw std::out_of_range("Invalid erase range");
            }
            std::size_t num_elements_to_erase = pos_end - pos_start;
            //std::move(pos_end, data + current, pos_start);
            current -= num_elements_to_erase;
            return pos_start;
        }
    };
}