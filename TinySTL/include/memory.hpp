#pragma once
#ifndef _MEMORY_HPP_
#define _MEMORY_HPP_
#include <type_traits>

namespace awstl
{
    template <typename T>
    constexpr decltype(auto) move(T&& t) noexcept
    {
        return static_cast<typename  std::remove_reference<T>::type &&>(t);
    }
    
    template <typename T>
    constexpr decltype(auto) forward(typename std::remove_reference<T>::type& t) noexcept
    {
        return static_cast<T&&>(t);
    }
}

#endif
