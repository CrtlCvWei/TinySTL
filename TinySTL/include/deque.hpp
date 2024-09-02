#pragma once
#ifndef _DEQUE_HPP_
#define _DEQUE_HPP_

#include <cstddef>
#include "allocator.hpp"
#include "iterator.hpp"
#include "uninitialized.hpp"

namespace awstl
{

#define INIT_SIZE 8
    
    template <class T, class Ref, class Ptr, size_t BufSize>
    struct _deque_iterator : aw_iterotor<T, ptrdiff_t, random_access_iterator_tag, Ptr, Ref>
    {
        using value_type = T;
        using reference = Ref;
        using const_reference = const Ref;
        using pointer = Ptr;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using iterator = _deque_iterator<T, T&, T*, BufSize>;
        using self = _deque_iterator; // 
        using const_iterator = _deque_iterator<T, const T&, const T*, BufSize>;
        using iterator_category = random_access_iterator_tag;
        using map_pointer = T**;
    
    protected:
        inline static size_type _dequq_buf_size(size_type buf_sz, size_type single)
        {
            return static_cast<size_type>(buf_sz > 0 ? buf_sz : (single < 512 ? 512 / single : 1));
        }
        
    public:
        map_pointer _map_node;
        T* cur;
        T* first;
        T* last;
        void set_node(map_pointer new_node)
        {
            _map_node = new_node;
            first = *new_node;
            last = first + static_cast<difference_type>(buff_size());
        }

#pragma region ctor
        _deque_iterator(): _map_node(nullptr), cur(nullptr), first(nullptr), last(nullptr) {}

        _deque_iterator(map_pointer node): _map_node(node), cur(*node), first(*node), last(*node + buff_size()) {}

        _deque_iterator(map_pointer node, T* pos): _map_node(node), cur(pos), first(*node), last(*node + buff_size()) {}

        _deque_iterator(const iterator& other): _map_node(other._map_node), cur(other.cur), first(other.first),
                                                last(other.last) {}
#pragma endregion

        static size_type buff_size() { return _dequq_buf_size(BufSize, sizeof(T)); }

        difference_type operator-( const self& other) const
        {
            return static_cast<difference_type>(
                buff_size() * (_map_node - other._map_node ) + (cur - first) - (other.cur - other.first));
        }

        void set_node(map_pointer _map, T* node)
        {
            _map_node = _map;
            cur = node;
            first = node;
            last = first + buff_size();
        }

#pragma region operation

        self& operator=(const self& other)
        {
            cur = other.cur;
            first = other.first;
            last = other.last;
            _map_node = other._map_node;
            return *this;
        }

        self& operator++()
        {
            ++cur;
            if (cur == last)
            {
                set_node(_map_node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--()
        {
            if (cur == first)
            {
                set_node(_map_node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int)
        {
            self tmp = *this;
            --*this;
            return tmp;
        }

        self& operator+=(difference_type n)
        {
            difference_type dist = distance(last , cur);
            if (n < dist)
            {
                cur += n;
            }
            else
            {
                difference_type offset = n > 0 ? n / buff_size() : (n + 1) / buff_size() - 1;
                set_node(_map_node + offset);
                cur = first + n % buff_size();
            }
            return *this;
        }

        self operator+(difference_type n)
        {
            self tmp = *this;
            tmp += n;
            return tmp;
        }

        self& operator-=(difference_type n)
        {
            return *this += -n;
        }

        self operator-(difference_type n)
        {
            self tmp = *this;
            tmp -= n;
            return tmp;
        }

        reference operator*() const
        {
            return *cur;
        }

        self& operator[](difference_type n)
        {
            return *(*this + n);
        }

        bool operator==(const self& other) const
        {
            return cur == other.cur;
        }

        bool operator!=(const self& other) const
        {
            return cur != other.cur;
        }

        bool operator<(const self& other) const
        {
            return _map_node == other._map_node ? cur < other.cur : _map_node < other._map_node;
        }

        bool operator>(const self& other) const
        {
            return other < *this;
        }

#pragma endregion
    };

    template <class T, class Alloc = allocator<T>, size_t BufSize = 0>
    class deque
    {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using iterator = _deque_iterator<T, T&, T*, BufSize>;
        using const_iterator = _deque_iterator<T, const T&, const T*, BufSize>;
        using allocator = Alloc;
        using self = deque<T, Alloc, BufSize>;

    protected:
        using map_pointer = pointer*;

        size_type map_size;
        map_pointer map;
        iterator start;
        iterator finish;

    protected:
        void fill_initialize(size_type n, const T& value);
        void create_map_and_nodes(size_type n);
        void rellocate_map(size_type nodes_to_add, bool add_at_front );

    public:
#pragma region ctor
        deque():deque(1, T()){}
        deque(int n) { auto value = T(); this->fill_initialize(n, value);}
        deque(int n, const T& value) {this->fill_initialize(n, value);}
        template <class IIter>
        deque(IIter first, IIter last)
        {
            create_map_and_nodes(static_cast<size_type>(last - first));
            auto cur = begin().cur;
            try
            {
                for(auto it = first; it != last; ++it)
                {
                    allocator::construct(cur++, *it);
                }
            }
            catch (...)
            {
                for(; cur != first; --cur)
                {
                    allocator::destroy(cur);
                }
                awstl::allocator<T*>::deallocate(map, map_size);
            }
        }
        deque(const deque& other)
        {
            create_map_and_nodes(other.size());
            try
            {
                uninitialized_copy(other.cbegin(), other.cend(), start, finish);
            }
            catch (...)
            {
                clear();
                awstl::allocator<T*>::deallocate(map, map_size);
            }
        }
#pragma endregion
        map_pointer GetMap() { return map; }
        iterator begin() { return start; }
        reference operator[](size_type n) { return start[n]; }
        iterator end() { return finish; }
        iterator cbegin() const {return start;}
        iterator cend() const {return finish;}
        reference front() { return *start; }
        reference back() { auto tmp = finish; --tmp; return *tmp;}
        size_type size() const { return finish - start; }
        size_type buff_size() const { return iterator::buff_size(); }

        bool empty() const { return finish == start; }
        void push_back(const T& value);
        void push_back_aux(const T& value);
        void push_front(const T& value);
        void push_front_aux(const T& value);
        void reserve_map_at_back(size_type nodes_to_add = 1); // 判断是否在尾部添加节点
        void reserve_map_at_front(size_type nodes_to_add = 1); // 判断是否在头部添加节点
        void pop_back();
        void pop_front();
        void clear();
    };
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::fill_initialize(size_type n, const T& value)
{
    if(n <= 0) std::cerr << "Are u sure ? "<< std::endl;
    create_map_and_nodes(n);
    auto cur = begin()._map_node;
    try
    {
        for(; cur != end()._map_node; ++cur)
        {
            uninitialized_fill(*cur, *cur + buff_size(), value);
        }
        uninitialized_fill(finish.first, finish.cur, value);
    }
    catch (...)
    {
        for(auto p = map; p < cur; ++p)
        {
            allocator::deallocate(*p, buff_size());
        }
    }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::create_map_and_nodes(size_type n)
{
    size_type node_num = n / buff_size() + 1;
    map_size = std::max(node_num + 2, static_cast<size_type>(INIT_SIZE) );
    map = awstl::allocator<T*>::allocate(map_size);
    for(size_type i = 0; i < map_size; ++i)
    {
        *(map + i) = nullptr;
    }

    auto tmpstart = map + (map_size - node_num) / 2;
    auto tmpfinish = tmpstart + node_num - 1;
    map_pointer cur;
    try
    {
        for(cur = tmpstart; cur <= tmpfinish; ++cur)
        {
            *cur = allocator::allocate(buff_size());
        }
        start.set_node(tmpstart);
        finish.set_node(tmpfinish);
        start.cur = start.first;
        finish.cur = finish.first + n % buff_size() ;
    }
    catch (...)
    {
        while(cur != tmpstart)
        {
            --cur;
            allocator::destroy(*cur, buff_size());
        }
        awstl::allocator<T*>::deallocate(map, map_size);
        map = nullptr;
        map_size = 0;
        throw;
    }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::rellocate_map(size_type nodes_to_add, bool add_at_front)
{
    auto old_nodes_num = finish._map_node - start._map_node + 1;
    auto new_nodes_num = old_nodes_num + nodes_to_add;

    map_pointer nstart = nullptr;
    if(map_size > 2 * new_nodes_num)
    {
        // 不用重新分配了，转移一下原来map里的地址
        nstart = map + (map_size - new_nodes_num)/2 + (add_at_front ? nodes_to_add : 0);
        if(nstart < start._map_node)
        {
            std::copy(start._map_node, finish._map_node + 1, nstart);
        }
        else
        {
            std::copy_backward(start._map_node, finish._map_node + 1, nstart + old_nodes_num);
        }
    }
    else
    {
        size_type new_map_size =  map_size +  std::max(map_size ,  nodes_to_add) + 2;
        map_pointer new_map = awstl::allocator<T*>::allocate(new_map_size);
        nstart = new_map + (new_map_size - new_nodes_num) / 2 + (add_at_front ? nodes_to_add : 0);
        std::copy(start._map_node, finish._map_node + 1, nstart);
         awstl::allocator<T*>::deallocate(map, map_size);
        map = new_map;
        map_size = new_map_size;
    }
        start.set_node(nstart);
        finish.set_node(nstart + old_nodes_num - 1);
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::push_back(const T& value)
{
    if(finish.cur != finish.last - 1)
    {
        allocator::construct(finish.cur++, value);
    }
    else
        push_back_aux(value);
    
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::push_back_aux(const T& value)
{
    // 只有当finish.cur == finish.last - 1时才会调用

    reserve_map_at_back();
    *(finish._map_node + 1) = allocator::allocate(buff_size());
    try
    {
        allocator::construct(finish.cur, value);
        finish.set_node(finish._map_node + 1);
        finish.cur = finish.first;
    }
    catch (...)
    {
        allocator::deallocate(*(finish._map_node + 1), buff_size());
    }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::push_front(const T& value)
{
    if(start.cur != start.first)
    {
        allocator::construct(--start.cur, value);
    }
    else
    {
        push_front_aux(value);
    }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::push_front_aux(const T& value)
{
    reserve_map_at_front();
    T* new_node = nullptr;
    try
    {
        new_node = allocator::allocate(buff_size());
        allocator::construct(new_node + buff_size() - 1, value);
    }
    catch (...)
    {
        allocator::deallocate(new_node, buff_size());
        throw;
    }
    *(start._map_node - 1) = new_node;
    start.set_node(start._map_node - 1);
    start.cur = start.last - 1;
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::reserve_map_at_back(size_type nodes_to_add)
{
    // 判断是否在尾部添加节点
   if(nodes_to_add + 1 > map_size - static_cast<size_type>(finish._map_node - map))
   {
       rellocate_map(nodes_to_add, false);
   }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::reserve_map_at_front(size_type nodes_to_add)
{
    // 判断是否在尾部添加节点
    if(nodes_to_add > static_cast<size_type>(start._map_node - map))
    {
        rellocate_map(nodes_to_add, true);
    }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::pop_back()
{
    if(finish.cur != finish.first)
    {
        --finish.cur;
        allocator::destroy(finish.cur);
    }
    else
    {
        allocator::destroy(finish.cur);
        allocator::deallocate(finish.first, buff_size());
        finish.set_node(finish ._map_node - 1);
        finish.cur = finish.last - 1;
    }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::pop_front()
{
    if(start.cur != start.last - 1)
    {
        allocator::destroy(start.cur);
        ++start.cur;
    }
    else
    {
        allocator::destroy(start.cur);
        allocator::deallocate(*start._map_node, buff_size());
        start.set_node(start._map_node + 1);
        start.cur = start.first;
    }
}

template <class T, class Alloc, size_t BufSize>
void awstl::deque<T, Alloc, BufSize>::clear()
{
    for(auto node = start._map_node + 1 ; node != finish._map_node; ++node)
    {
        allocator::destroy(*node, buff_size());
        allocator::deallocate(*node, buff_size());
    }
    //now only two nodes left
    if(start._map_node != finish._map_node)
    {
        allocator::destroy(start.cur, buff_size());
        allocator::destroy(finish.cur, buff_size());
        allocator::deallocate(finish.first, buff_size());
    }
    else
    {
        allocator::destroy(start.cur, buff_size());
    }
    finish = start;
}


#endif // _DEQUE_HPP_
