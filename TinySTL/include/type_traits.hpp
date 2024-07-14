#pragma once
#ifndef _TYPE_TRAITS_HPP_
#define _TYPE_TRAITS_HPP_

/*  used for traiting types for class T or T*  */
namespace awstl
{
    template <class T, T v>
    struct aw_integral_constant
    {
        static constexpr T value = v;
    };

    template <bool B>
    using aw_bool_constant = aw_integral_constant<bool,B>;
    typedef  aw_bool_constant<true> aw_true_type;
    typedef  aw_bool_constant<false> aw_false_type;
}


#endif // _TYPE_TRAITS_HPP_
