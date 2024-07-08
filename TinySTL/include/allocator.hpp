#pragma once
#ifndef TINYSTL_ALLOCATOR_H_
#define TINYSTL_ALLOCATOR_H_


#include "constructor.hpp"
#include <new>
#include  <cstddef>
#include <cstdlib>
#include <iostream>

#define throw_bad_alloc \
        std::cerr << "out of memory " << std::endl;\
        exit(1);
#define throw_bad_delloc\
        std::cerr << "fail to deallocate memory" << std::endl;\
        exit(1);
#define Max_malloc_try 10

namespace awstl
{
    /* NOTE: In C++17: We Should Not missing the comparison operators != / == */
    /* NOTE: We have ignored the multi-threading issue */

    template<typename... Args>
    static void* aw_general_oom_handler(void* (*oom_handler)(Args...), Args... args) {
        void* result = nullptr;
        size_t count = 0;

        while (result == nullptr && count < Max_malloc_try) {
            result = oom_handler(args...);
            ++count;
            if (result) return result;
        }
        return nullptr;
    }

    static  void * aw_oom_malloc(size_t size)
    {
        auto result = aw_general_oom_handler(std::malloc,size);
        if(!result)
            throw_bad_alloc;
    }

    static  void * aw_oom_realloc(void *ptr, size_t size)
    {
        auto result = aw_general_oom_handler(std::realloc,ptr,size);
        if(!result)
            throw_bad_alloc;
    }
    
    static  void _awHandler()
    {
        //TODO: Do something when memory allocation failed
        throw_bad_alloc;
    }
    
    template <class T>
    inline T* _allocate(ptrdiff_t size, T*)
    {
        std::set_new_handler(static_cast<std::new_handler>(_awHandler));
        try
        {
            if(size == 0)
                return nullptr;
            T* temp = static_cast<T*>(::operator new(size * sizeof(T)));
            if (temp == nullptr)
            {
                temp = static_cast<T*>(aw_oom_malloc(size * sizeof(T)));
            }
            return temp;
        }
        catch (const std::bad_alloc& e)
        {
           std::cerr << e.what() << std::endl;
            return nullptr;
        }
    }

    template <class T>
    inline T* _reallocate(T* ptr , ptrdiff_t size,T*)
    {
        std::set_new_handler(static_cast<std::new_handler>(_awHandler));
        try
        {
            if(size == 0)
                return nullptr;
            T* temp = static_cast<T*>(::operator new(size * sizeof(T),(void*)ptr));
            if (temp == nullptr)
            {
                temp = static_cast<T*>(aw_oom_realloc((void*)ptr,size * sizeof(T)));
            }
            return temp;
        }
        catch (const std::bad_alloc& e)
        {
            std::cerr << e.what() << std::endl;
            return nullptr;
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
            throw_bad_delloc;
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
            throw_bad_delloc;
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
    inline void _destroy(T* ptr) {
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
        static const_pointer const_address(reference x){return const_cast<const_pointer>(&x); }
        
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
       awstl:: _destroy(ptr);
    }
    
    /* NOTE: In C++17: We Should Not missing the comparison operators != / == */
    template <class T, class U>
    bool operator==(const allocator<T>&, const allocator<U>&) { return true; }
    template <class T, class U>
    bool operator!=(const allocator<T>&, const allocator<U>&) { return false; }
}

#endif
