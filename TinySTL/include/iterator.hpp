#pragma once
#ifndef _ITERATOR_HPP_
#define _ITERATOR_HPP_
#include <cstddef>


namespace awstl
{
    template <class T, class distance = ptrdiff_t>
    struct aw_iterator_traits
    {
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = distance;
        // using iterator_category = std::random_access_iterator_tag;
    };

    // partial specialization for pointer
    template <class T>
    struct aw_iterator_traits<T*>
    {
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        // using iterator_category = std::random_access_iterator_tag;
    };

    template <class T>
    typename aw_iterator_traits<T>::difference_type
    distance(T first, T last)
    {
        typename aw_iterator_traits<T>::difference_type n = 0;
        for (; first != last; ++first)
            ++n;
        return n;
    }

    template <class T>
    typename aw_iterator_traits<T>::difference_type
    count(T first, T last, const T& value)
    {
        typename aw_iterator_traits<T>::difference_type n = 0;
        for (; first != last; ++first)
        {
            if (*first == value)
                ++n;
        }
        return n;
    }
}


#endif // _ITERATOR_HPP_
