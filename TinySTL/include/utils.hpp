#pragma once
#ifndef _UTILS_HPP_
#define _UTILS_HPP_

namespace awstl
{
#define _ALIGN 8

    template <typename T>
    struct identity
    {
        template <typename T>
        constexpr T&& operator()(T&& t) const noexcept
        {
            return t;
        }
    };

    static size_t round_up(size_t bytes, size_t align = _ALIGN)
    {
        return ((bytes) + align - 1) & ~(align - 1);
    }

    static size_t round_down(size_t bytes, size_t align = _ALIGN)
    {
        return (bytes) & ~(align - 1);
    }

    static size_t mod(size_t bytes, size_t align = _ALIGN)
    {
        return bytes & (align - 1);
    }

    static constexpr size_t count_one_bits(size_t bytes)
    {
        auto count = 0;
        while (bytes)
        {
            ++count;
            bytes &= (bytes - 1);
        }
        return count;
    }

    static size_t find_highest_bit(size_t bytes)
    {
        auto count = 0;
        while (bytes)
        {
            ++count;
            bytes >>= 1;
        }
        return count;
    }
}


#endif // _UTILS_HPP_
