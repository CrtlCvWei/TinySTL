#pragma once
#ifndef _UTILS_HPP_
#define _UTILS_HPP_

namespace awstl
{
#define _ALIGN 8

    static size_t round_up(size_t bytes, size_t align = _ALIGN)
    {
        return ((bytes) + align - 1) & ~(align - 1);
    }

    static size_t round_down(size_t bytes, size_t align = _ALIGN)
    {
        return (bytes) & ~(align - 1);
    }

#pragma region move
    template <class T>
    auto && move(T && t) -> decltype(static_cast<T &&>(t))
    {
        return static_cast<typename std::remove_reference<T> &&>(t);
    }
#pragma endregion

#pragma region forward
    template <class T>
    auto && forward(typename std::remove_reference<T>::type & t) noexcept
    {
        return static_cast<T &&>(t);
    }
}


#endif // _UTILS_HPP_
