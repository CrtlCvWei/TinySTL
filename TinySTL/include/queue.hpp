#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include "list.hpp"

namespace awstl
{
    template<class T, class Sequence = awstl::list<T>>
    class queue
    {
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
        queue(): c_(){}
        queue(size_type n): c_(n){};
        queue(size_type n, const value_type& value): c_(n, value){};
        template <class IIter>
        queue(IIter first, IIter last): c_(first, last){};
        queue(std::initializer_list<T> list): c_(list.begin(), list.end()){};
        queue(const container_type& c): c_(c){};
#pragma endregion

        size_type size() const
        {
            return c_.size();
        }

        constexpr bool empty() const
        {
            return c_._size == 0;
        }

        reference front()
        {
            return c_.front();
        }

        reference back()
        {
            return c_.back();
        }

        void push(const value_type& value)
        {
            c_.push_back(value);
        }

        void pop()
        {
            c_.pop_front();
        }
    };
    
    
}


#endif
