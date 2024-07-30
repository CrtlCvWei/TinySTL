#pragma once
#ifndef _UNINITIALIZED_HPP_
#define _UNINITIALIZED_HPP_

#include <type_traits>

#include "iterator.hpp"
#include "type_traits.hpp"

/*定义了一些全局函数，用来fill or copy 大块内存数据*/
namespace awstl
{
    template <typename T>
    struct is_pod
    {
        /* C++20 后的写法 */
        static constexpr bool value = std::is_trivially_copyable<T>::value && std::is_standard_layout<T>::value;
    };
# pragma region uninitialized_fill_n

    template <class ForwardIter, class Size, class T>
    inline ForwardIter _uninitialized_fill_aux(ForwardIter first, Size n, const T& x, aw_true_type)
    {
        return std::fill_n(first, n, x);
    }

    template <class ForwardIter, class Size, class T>
    inline ForwardIter _uninitialized_fill_aux(ForwardIter first, Size n, const T& x, aw_false_type)
    {
        if (n <= 0)
            return first;
        ForwardIter cur = first;
        try
        {
            for (; n > 0; --n, ++cur)
            {
                awstl::_construct(&*cur, x);
            }
            return cur;
        }
        catch (...)
        {
            for (; first != cur; ++first)
            {
                cur->~T();
            }
            return cur;
        }
    }

    template <class ForwardIter, class Size, class T, class T1>
    inline ForwardIter _uninitialized_fill_n(ForwardIter first, Size n, const T& x, T1*)
    {
        constexpr auto is_pod = awstl::is_pod<T1>::value;
        return _uninitialized_fill_aux(first, n, x, awstl::aw_bool_constant<is_pod>{});
    }

    template <class ForwardIter, class Size, class T>
    inline ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& x)
    {
        return _uninitialized_fill_n(first, n, x, awstl::value_type(first));
    }
#pragma endregion

#pragma region uninitialized_copy
    /* warning:  (last - first) must be less than (<result>end() - result) */
    template <class InputIter, class ForwardIter>
    inline  ForwardIter _uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, aw_true_type) {
        return std::copy(first, last, result);
    }
    
    template <class InputIter, class ForwardIter>
    inline ForwardIter _uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, aw_false_type)
    {
        ForwardIter curr = result;
        auto type = std::iterator_traits<ForwardIter>::value_type;
        try
        {
            for (; first != last && curr ; ++first, ++curr)
            {
                awstl::_construct(&*curr, *first);
            }
            return curr;
        }
        catch (...)
        {
            for (; result != curr; ++result)
            {
                curr->~T();
            }
            return result;
        }
    }

    template <class InputIter, class ForwardIter, class T>
    inline ForwardIter _uninitialized_copy(InputIter first, InputIter last, ForwardIter result, T*)
    {
        constexpr bool is_pod = awstl::is_pod<T>::value;
        return _uninitialized_copy_aux(first, last, result, aw_bool_constant<is_pod>{});
    }

    template <class InputIter, class ForwardIter>
    inline ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result,ForwardIter result_last)
    {
        if(std::distance(result, result_last) < std::distance(first, last))
        {
            auto last_new = first + std::distance(result, result_last);
            return _uninitialized_copy(first,last_new,result,awstl::value_type(first));
        }
        return _uninitialized_copy(first, last, result, awstl::value_type(first));
    }

#pragma endregion

#pragma region uninitialized_fill

    template <class ForwardIterator, class T>
    inline  void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, aw_true_type)
    {
        std::fill(first, last, x);
    }

    template <class ForwardIterator, class T>
    inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, aw_false_type)
    {
        ForwardIterator curr = first;
        try
        {
            for (; curr != last; ++curr)
            {
                awstl::_construct(&*curr, x);
            }
        }
        catch (...)
        {
            for (; first != curr; ++first)
            {
                curr->~T();
            }
        }
    }

    template <class ForwardIterator, class T>
    inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T*)
    {
        constexpr bool is_pod_type = awstl::is_pod<T>::value;
        _uninitialized_fill_aux(first, last, x, aw_bool_constant<is_pod_type>{});
    }

    template <class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
    {
        _uninitialized_fill(first, last, x, awstl::value_type(first));
    }

#pragma endregion
}


#endif
