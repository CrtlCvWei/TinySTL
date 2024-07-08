#pragma once
#ifndef TINYSTL_CONSTRUCTOR_H_
#define TINYSTL_CONSTRUCTOR_H_
#include <new>
#include <type_traits>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100) // unused parameter
#endif                          // _MSC_VER

namespace awstl
{
    // TODO: constructor template for construct object
#pragma region constructor

    template <class T>
    void construct(T* ptr)
    {
        // 使用 placement new 在指定内存地址 ptr 处构造一个 T 类型的对象
        ::new(static_cast<void*>(ptr)) T();
    }

    template <class T1, class T2>
    void construct(T1* ptr, const T2& value)
    {
        // 使用 placement new 在指定内存地址 ptr 处构造一个 T1 类型的对象,并用 value 初始化
        ::new(static_cast<void*>(ptr)) T1(value);
    }

    template <class T1, class... Args>
    void construct(T1* ptr, Args&&... args)
    {
        // TODO : replace std::forward to my::forward
        ::new(static_cast<void*>(ptr)) T1(std::forward<Args>(args)...);
    }
#pragma endregion

#pragma region destructor
    template <class T>
    void destroy_one(T* ptr, std::true_type){/* let the compiler handle this type */};

    template <class T>
    void destroy_one(T* ptr, std::false_type)
    {
        if (ptr == nullptr)
            return;
        ptr->~T();
    }

    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::true_type){/* let the compiler handle this type */};

    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
    {
        using value_type = typename std::iterator_traits<ForwardIter>::value_type;
        for (; first != last; ++first)
        {
            // first 是一个迭代器，*first 是一个对象
            destroy(&*first);
        }
    }
    
    template <class T>
    void destroy(T* ptr)
    {
        // 封装
        awstl::destroy_one(ptr, std::is_trivially_destructible<T>());
    }


    template <class ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        using value_type = typename std::iterator_traits<ForwardIter>::value_type;
        destroy_cat(first, last, std::is_trivially_destructible<value_type>());
    };

    template <class ForwardIter,class T>
    void destroy(ForwardIter first, ForwardIter last, T*)
    {
        using _trivial_destructor = std::is_trivially_destructible<T>;
        destroy_cat(first, last, _trivial_destructor());
    }

    inline void destroy(char*, char*){}
    inline void destroy(wchar_t*, wchar_t*){}
#pragma endregion
    
}

#endif
