#pragma once
#ifndef _ITERATOR_HPP_
#define _ITERATOR_HPP_
#include <cstddef>


namespace awstl
{
    // 五种迭代器类型
    struct input_iterator_tag{};

    struct output_iterator_tag{};

    struct forward_iterator_tag : public input_iterator_tag{};

    struct bidirectional_iterator_tag : public forward_iterator_tag{};

    struct random_access_iterator_tag : public bidirectional_iterator_tag{};


    template <class T,
    class Distance = ptrdiff_t,
    class Category = random_access_iterator_tag ,
    class Pointer = T*,
    class Reference = T&>
    struct aw_iterotor
    {
        typedef T value_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Distance difference_type;
        typedef Category iterator_category;
    };
    
    template <class Iterator>
    struct aw_iterator_traits
    {
        using value_type = typename Iterator::value_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
        using difference_type =  typename Iterator::difference_type;
        using iterator_category = typename Iterator::iterator_category;
    };

    // partial specialization for pointer
    template <class T>
     struct aw_iterator_traits<T*>
    {
        using value_type = T;
        using pointer = T* ;
        using reference = T& ;
        using difference_type = ptrdiff_t;
        using iterator_category = random_access_iterator_tag;
    };

    // partial specialization for const pointer
    template <class T>
      struct aw_iterator_traits<const T*>
    {
        using value_type = T;
        using pointer = T* ;
        using reference = T& ;
        using difference_type = ptrdiff_t;
        using iterator_category = random_access_iterator_tag;
    };

#pragma region traits

    template <class Iterator>
    inline  typename aw_iterator_traits<Iterator>::value_type*
        value_type(const Iterator&)
    {
        return static_cast<typename aw_iterator_traits<Iterator>::value_type*> (nullptr);
    }

    template <class Iterator>
    inline typename aw_iterator_traits<Iterator>::difference_type
        difference_type(const Iterator&)
    {
        return static_cast<typename aw_iterator_traits<Iterator>::difference_type>(nullptr);
    }

    template <class Iterator>
    inline typename aw_iterator_traits<Iterator>::pointer
        pointer(const Iterator&)
    {
        return static_cast<typename aw_iterator_traits<Iterator>::pointer>(nullptr);
    }

    template <class Iterator>
    inline typename aw_iterator_traits<Iterator>::reference
        reference(const Iterator&)
    {
        return static_cast<typename aw_iterator_traits<Iterator>::reference>(nullptr);
    }

    template <class Iterator>
    inline typename aw_iterator_traits<Iterator>::iterator_category
        iterator_category(const Iterator&)
    {
        return static_cast<typename aw_iterator_traits<Iterator>::iterator_category()>(nullptr);
    }
#pragma endregion

    
#pragma region distance
    template <class InputIterator>
    typename aw_iterator_traits<InputIterator>::difference_type
    _distance(InputIterator first, InputIterator last,input_iterator_tag)
    {
        typename aw_iterator_traits<InputIterator>::difference_type n = 0;
        for (; first != last; ++first)
            ++n;
        return n;
    }

    template <class InputIterator>
    typename aw_iterator_traits<InputIterator>::difference_type
    _distance(InputIterator first, InputIterator last,random_access_iterator_tag)
    {

        return last - first;
    }

    template <class InputIterator>
    typename aw_iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last)
    {
        return _distance(first, last, Get_It_cat(static_cast<InputIterator>(nullptr)));
    }

#pragma endregion

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
    
#pragma region advance
    template <class ForwardIterator, class Distance>
    inline void _advance(ForwardIterator& it, Distance n, input_iterator_tag)
    {
        while (n--) ++it;
    }

    template <class ForwardIterator, class Distance>
    inline void _advance(ForwardIterator& it, Distance n, forward_iterator_tag)
    {
       _advance(it, n, input_iterator_tag());
    }

    template <class ForwardIterator, class Distance>
    inline void _advance(ForwardIterator& it, Distance n, bidirectional_iterator_tag)
    {
        if (n == 0) return;
        else if (n > 0)
           { while (n--) ++it;}
        else
            while (n++) --it;
    }

    template <class RandomIterator, class Distance>
    inline void _advance(RandomIterator& it, Distance n, random_access_iterator_tag)
    {
        it += n;
    }

    template <class InputIterator, class Distance>
    inline void advance(InputIterator& it, Distance n)
    {
        _advance(it, n, Get_It_cat(static_cast<InputIterator>(nullptr)));
    }
#pragma endregion
    
    // 代表反向迭代器，使前进为后退，后退为前进
    template <class Iterator>
    class reverse_iterator
    {
    private:
        Iterator current ; // 记录对应的的正向迭代器 
    public:
        using value_type = typename Iterator::value_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
        using difference_type =  typename Iterator::difference_type;
        using iterator_category = typename Iterator::iterator_category;

        using iterator_type = Iterator;
        using self = reverse_iterator<Iterator>;
    public:
        reverse_iterator() = default;
        reverse_iterator(const Iterator& it): current(it) {};
        reverse_iterator(const self& it): current(it.current) {};
    public:
        Iterator base() const { return current; }
        reference operator*() const
        {
            Iterator tmp = current;
            return *--tmp;
        }
        pointer operator->() const
        {
            return &(operator*());
        }
        
        self& operator = (const self& it)
        {
            current = it.current;
            return *this;
        }
        
        self& operator++()
        {
            --current;
            return *this;
        }
        self& operator--()
        {
            ++current;
            return *this;
        }
        self operator++(int)
        {
            auto temp = *this;
            --current;
            return temp;
        }
        self operator--(int)
        {
            auto temp = *this;
            ++current;
            return temp;
        }
        self& operator+=(difference_type n)
        {
            current -= n;
            return *this;
        }
        self& operator-=(difference_type n)
        {
            current += n;
            return *this;
        }
        self operator+(difference_type n) const
        {
            return self(current - n);
        }
        self operator-(difference_type n) const
        {
            return self(current + n);
        }
        reference operator[](difference_type n)
        {
            return *(*this + n);
        }

        // 重载 operator-
        template <class Iterator>
        typename reverse_iterator<Iterator>::difference_type
        operator-(const reverse_iterator<Iterator>& rhs)
        {
            return this->base() - rhs.base();
        }
        // 重载 operator-
        template <class Iterator>
        typename reverse_iterator<Iterator>::difference_type
        operator+(const reverse_iterator<Iterator>& rhs)
        {
            return this->base() + rhs.base();
        }

        // 重载 operator==
        template <class Iterator>
        typename reverse_iterator<Iterator>::difference_type
        operator==(const reverse_iterator<Iterator>& rhs)
        {
            return this->base() == rhs.base();
        }
        
        // 重载 operator-
        template <class Iterator>
        typename reverse_iterator<Iterator>::difference_type
        operator!=(const reverse_iterator<Iterator>& rhs)
        {
            return !(this == rhs);
        }



        
    };
    
}


#endif // _ITERATOR_HPP_
