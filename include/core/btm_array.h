#pragma once

namespace btm {
    // fixed size ppg_array class, similar to std::array but simpler
    template <typename T, size_t N>
    class btm_array {
        T m_data[N];
    public:
        btm_array() = default;
        ~btm_array() = default;
        size_t size() const { return N; }
        T& operator[](size_t index) { return m_data[index]; }
        const T& operator[](size_t index) const { return m_data[index]; }
        T* data() { return m_data; }
        const T* data() const { return m_data; }
        // provide begin() and end() methods for range-based for loops
        T* begin() { return m_data; }
        T* end() { return m_data + N; } // some pointer arithmetic to get the end pointer
        const T* begin() const { return m_data; }
        const T* end() const { return m_data + N; }
    };

}