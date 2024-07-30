#pragma once
#ifndef TINYSTL_CONSTRUCTOR_H_
#define TINYSTL_CONSTRUCTOR_H_
#include <new>
#include <type_traits>

#include "type_traits.hpp"

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
    void destroy_one(T* ptr, aw_true_type){/* let the compiler handle this type */};

    template <class T>
    void destroy_one(T* ptr, aw_false_type)
    {
        if (ptr == nullptr)
            return;
        ptr->~T();
    }

    template <class T>
    void destroy(T* ptr)
    {
        // 封装
        awstl::destroy_one(ptr,_type_traits<T>::has_trivial_destructor());
    }

    template <class ForwardIter>
  void destroy_cat(ForwardIter first, ForwardIter last, aw_true_type){/* let the compiler handle this type */};
    
    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, aw_false_type)
    {
        using value_type = typename std::iterator_traits<ForwardIter>::value_type;
        for (; first != last; ++first)
        {
            // first 是一个迭代器，*first 是一个对象
            destroy(&*first);
        }
    }
    
    template <class ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        destroy_cat(first, last, awstl::_type_traits<ForwardIter>::has_trivial_destructor());
    };

    template <class ForwardIter,class T>
    void destroy(ForwardIter first, ForwardIter last, T*)
    {
        destroy_cat(first, last, awstl::_type_traits<ForwardIter>::has_trivial_destructor());
    }

    inline void destroy(char*, char*){}
    inline void destroy(wchar_t*, wchar_t*){}
#pragma endregion
    
}

#endif
