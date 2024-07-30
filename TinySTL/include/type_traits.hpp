#pragma once
#ifndef _TYPE_TRAITS_HPP_
#define _TYPE_TRAITS_HPP_

#define _TEMPLATE_NULL template<>
#define NOT_TRIVIAL_TYPE_TRAITS \
using  has_trivial_default_constructor = aw_false_type; \
using  has_trivial_copy_constructor = aw_false_type; \
using  has_trivial_assignment_operator = aw_false_type; \
using has_trivial_destructor = aw_false_type; \
using  is_POD_type = aw_false_type; 

#define TRIVIAL_TYPE_TRAITS \
using  has_trivial_default_constructor = aw_true_type; \
using  has_trivial_copy_constructor = aw_true_type; \
using  has_trivial_assignment_operator = aw_true_type; \
using  has_trivial_destructor = aw_true_type; \
using  is_POD_type = aw_true_type; 


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


    template <class type>
    struct _type_traits 
    {
        typedef aw_true_type this_dummy_member_must_be_first;
        NOT_TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<char>
    {
       TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<signed char>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<unsigned char>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<short>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<unsigned short>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<int>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<unsigned int>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<long>
    {
        TRIVIAL_TYPE_TRAITS
    };
    
    _TEMPLATE_NULL struct _type_traits<float>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<double>
    {
        TRIVIAL_TYPE_TRAITS
    };

    _TEMPLATE_NULL struct _type_traits<long double>
    {
        TRIVIAL_TYPE_TRAITS
    };
    
    template <class type>
    typename _type_traits<type>::has_trivial_default_constructor _type_traits_default_constructor(const type& t)
    {
        return typename _type_traits<type>::has_trivial_default_constructor();
    }

    template <class type>
    typename _type_traits<type>::has_trivial_copy_constructor _type_traits_copy_constructor(const type& t)
    {
        return typename _type_traits<type>::has_trivial_copy_constructor();
    }

    template <class type>
    typename _type_traits<type>::has_trivial_assignment_operator _type_traits_assignment_operator(const type& t)
    {
        return typename _type_traits<type>::has_trivial_assignment_operator();
    }

    template <class type>
    typename _type_traits<type>::has_trivial_destructor _type_traits_destructor(const type& t)
    {
        return typename _type_traits<type>::has_trivial_destructor();
    }

    template <class type>
    typename _type_traits<type>::is_POD_type _type_traits_POD(const type& t)
    {
        return typename _type_traits<type>::is_POD_type();
    }
}


#endif // _TYPE_TRAITS_HPP_
