
#ifndef __STACK_H_
#define __STACK_H_

#include "vector.h"

template <typename T>
class Stack : public Vector<T> {
private:
    Vector<T> datas;
};

#endif // __STACK_H_
