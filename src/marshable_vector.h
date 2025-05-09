template <typename T>
struct mashable_vector {
    T* m_data;
    unsigned int m_size;

    T* operator[](unsigned int index) {
        assert(index < m_size && m_data != nullptr);
        return &m_data[index];
    }
};