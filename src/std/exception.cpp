
#include "exception.h"

using namespace std;

exception::~exception() {};

const char *exception::what()
{
    return nullptr;
}

const char *bad_alloc::what() 
{
    return "bad_alloc";
}
