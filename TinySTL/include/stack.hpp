#ifndef _STACK_HPP_
#define _STACK_HPP_

#include "deque.hpp"
#include "memory.hpp"

namespace awstl
{
    // 模板类 stack
    // 参数一代表数据类型，参数二代表底层容器类型，缺省使用 awstl::deque 作为底层容器
    template <class T , class Sequence = deque<T>>
    class stack
    {
    public:
        using container_type = Sequence;
        using value_type = typename Sequence::value_type;
        using size_type = typename Sequence::size_type;
        using reference = typename Sequence::reference;
        using const_reference = typename Sequence::const_reference;
        static_assert(std::is_same<T, value_type>::value,
              "the value_type of Container should be same with T");
    private:
        container_type c_;

    public:

#pragma region ctor
        stack() = default;
        explicit stack(size_type n) : c_(n) {}
        stack(size_type n, const value_type& value) : c_(n, value) {}
        template <class IIter>
        stack(IIter first, IIter last): c_(first, last){}
        
        stack(std::initializer_list<T> list) :c_(list.begin(), list.end()){}
        stack(const container_type& c) : c_(c) {}
        stack(container_type&& c) noexcept : c_(std::move(c)) {}
        stack(const stack& rhs): c_(rhs.c_){}
        stack(stack&& rhs) noexcept(std::is_nothrow_move_constructible<container_type>::value): c_(awstl::move(rhs.c_)){} 
        
#pragma endregion

#pragma region operation
        stack& operator=(const stack& rhs)
        {
            c_ = rhs.c_;
            return *this;
        }

        stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<container_type>::value)
        {
            c_ = awstl::move(rhs.c_);
            return *this;
        }
        
        void swap(stack& rhs) noexcept(noexcept(std::swap(c_, rhs.c_)))
        {
            std::swap(c_, rhs.c_);
        }
        
        friend void swap(stack& lhs, stack& rhs) noexcept(noexcept(lhs.swap(rhs)))
        {
            lhs.swap(rhs);
        }
        
        bool operator==(const stack& rhs) const
        {
            return c_ == rhs.c_;
        }

        bool operator!=(const stack& rhs) const
        {
            return c_ != rhs.c_;
        }
        
        void clear()
        {
            c_.clear();
        }

    public:
        friend bool operator==(const stack& lhs, const stack& rhs) { return lhs.c_ == rhs.c_; }
        friend bool operator< (const stack& lhs, const stack& rhs) { return lhs.c_ <  rhs.c_; }
#pragma endregion
        constexpr bool empty() const
        {
            return c_.empty();
        }

        size_type size() const
        {
            return c_.size();
        }

        reference top()
        {
            return c_.back();
        }

        const_reference top() const
        {
            return const_cast<const_reference>(c_.back());
        }

        void push(const value_type& value)
        {
            c_.push_back(value);
        }

        void pop()
        {
            c_.pop_back();
        }
        
    };

    template <class T, class Container>
    bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return rhs < lhs;
    }

    template <class T, class Container>
    bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T, class Container>
    bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(lhs < rhs);
    }
}



#endif // !_STACK_HPP_