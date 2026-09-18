#pragma once

namespace btm {
    // fixed size btm_array class, similar to std::array but simpler
    template <typename T>
    class btm_array {
        T* m_data;
        size_t m_size;
    public:
        btm_array() : m_data(nullptr), m_size(0) {}
        btm_array(size_t size) : m_data(new T[size]), m_size(size) {}
        ~btm_array() { delete[] m_data; }
        void resize(size_t new_size) {
            delete[] m_data;
            m_data = new T[new_size];
            m_size = new_size;
        }
        size_t size() const { return m_size ; }
        T& operator[](size_t index) { return m_data[index]; }
        const T& operator[](size_t index) const { return m_data[index]; }
        T* data() { return m_data; }
        const T* data() const { return m_data; }
        // provide begin() and end() methods for range-based for loops
        T* begin() { return m_data; }
        T* end() { return m_data + m_size; } // some pointer arithmetic to get the end pointer
        const T* begin() const { return m_data; }
        const T* end() const { return m_data + m_size; }
    };
}