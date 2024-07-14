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
}


#endif // _UTILS_HPP_
