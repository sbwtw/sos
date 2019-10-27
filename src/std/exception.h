
#ifndef __EXCEPTION_H_
#define __EXCEPTION_H_

namespace std {
    class exception
    {
    public:
        exception() noexcept {}
        virtual ~exception();

        virtual const char *what() noexcept;
    };

    class bad_alloc : public exception 
    {
    public:
        virtual const char *what() noexcept;
    };
}

#endif // __EXCEPTION_H_
