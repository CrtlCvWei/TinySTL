#pragma once
#ifndef TINYSTL_ALLOCATOR_H_
#define TINYSTL_ALLOCATOR_H_

#include "constructor.hpp"
#include <new>
#include  <cstddef>
#include <cstdlib>
#include <iostream>

namespace awstl
{
    template <class T>
    inline T* _allocate(ptrdiff_t size, T*)
    {
        std::set_new_handler(0);
        try
        {
            if(size == 0)
                return nullptr;
            T* temp = static_cast<T*>(::operator new(size * sizeof(T)));
            return temp;
        }
        catch (const std::bad_alloc& e)
        {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }
    }

    template <class T>
    inline void _deallocate(T* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }
        try
        {
            ::operator delete(ptr);
        }
        catch (...)
        {
            std::cerr << "fail to deallocate memory" << std::endl;
        }
    }

    template <class T>
    inline void _deallocate(T* ptr, size_t n)
    {
        if (ptr == nullptr)
        {
            return;
        }
        try
        {
            ::operator delete(ptr, n);
        }
        catch (...)
        {
            std::cerr << "fail to deallocate memory" << std::endl;
        }
    }
    
    template <class T>
    inline void _construct(T* ptr)
    {
        awstl::construct(ptr);
    }
    
    template <class T1, class T2>
    inline void _construct(T1* ptr, const T2& value)
    {
        awstl::construct(ptr,value);
    }
    
    template <typename T>
    void destroy_one(T* ptr) {
        awstl::destroy(ptr);
    }
    
    template <class T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        
         static  pointer allocate();
         static  pointer allocate(size_type n);
         static  void deallocate(pointer ptr);
         static  void deallocate(pointer ptr, size_type n);
        
        static  void construct(pointer ptr);
        void construct(pointer ptr, const T& x);
        void construct(pointer ptr, T&& x);
        template <class... Args>
        void construct(pointer ptr, Args&&... args);
        void destroy(pointer ptr);

    public:
        allocator() = default;
        template<class U> constexpr allocator(const allocator<U>&) noexcept {}
        
        template <class U>
        struct rebind
        {
            typedef allocator<U> other;
        };
        
        static pointer address(reference x) { return &x; }
        
    };

    template <class T>
     T* allocator<T>::allocate()
    {
        return _allocate(1, static_cast<T*>(0));
    }

    template <class T>
    T* allocator<T>::allocate(size_type n)
    {
        return _allocate(n, static_cast<T*>(0));
    }

    template <class T>
    void allocator<T>::deallocate(T* ptr){ _deallocate(ptr);}
    
    template <class T>
    void allocator<T>::deallocate(T* ptr, size_t n){ _deallocate(ptr,n); }

    template <class T>
    void allocator<T>::construct(T* ptr)
    {
        _construct(ptr);
    }
    
    template <class T>
    void allocator<T>::construct(T* ptr, const T& x)
    {
        _construct(ptr, x);
    }

    template <class T>
    void allocator<T>::construct(T* ptr, T&& x)
    {
        _construct(ptr, std::move(x));
    }
    
    template <class T>
    template <class... Args>
    void allocator<T>::construct(pointer ptr, Args&&... args)
    {
        ::new(ptr) T(std::forward<Args>(args)...);
    }
    
    template <class T>
    void allocator<T>::destroy(pointer ptr)
    {
        destroy_one(ptr);
    }
    
    /* NOTE: In C++17: We Should Not missing the comparison operators != / == */
    template <class T, class U>
    bool operator==(const allocator<T>&, const allocator<U>&) { return true; }
    template <class T, class U>
    bool operator!=(const allocator<T>&, const allocator<U>&) { return false; }
}

#endif
