
#ifndef __VECTOR_H_
#define __VECTOR_H_

#include "types.h"

template <typename T>
class Vector {

public:
    explicit Vector();

    inline uint32_t size() const { return current_len; }
    inline uint32_t capacity() const { return storage_capacity; }

    void push_back(const T &val);
    //void push_back(T &&val);

protected:
    void grow_up();

private:
    uint32_t storage_capacity;
    uint32_t current_len;
    T *data_ptr;
};

template <typename T>
Vector<T>::Vector()
    : storage_capacity(0)
    , current_len(0)
    , data_ptr(nullptr)
{
}

template <typename T>
void Vector<T>::grow_up()
{
    //const auto old_size = current_len;
    //const auto new_size = storage_capacity + 10;
}


#endif // __VECTOR_H_
