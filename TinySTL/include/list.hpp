#pragma once
#ifndef _LIST_HPP_
#define _LIST_HPP_
#include "allocator.hpp"
#include "iterator.hpp"
#include "memory.hpp"
#include "vector.hpp"

// 这个头文件包含了一个模板类 list
// list : 双向链表

namespace awstl
{
    template <class T>
    struct _list_node
    {
        using value_type = T;
        _list_node* prev;
        _list_node* next;
        value_type data;

        _list_node(): prev(nullptr), next(nullptr), data(T()){}
        _list_node(const T& x): prev(nullptr), next(nullptr), data(x){}
        
        ~_list_node() { data.~T(); }

        _list_node* self() { return &*this; }
        void unlink() { prev = next = self(); }
    };

    template <class T, class Ref, class Ptr>
    struct _list_node_iterator
    {
        using list_node = _list_node<T>;
        using self = _list_node_iterator<T, T&, T*>;
        using reference = Ref;
        using pointer = Ptr;
        using iterator_category = bidirectional_iterator_tag;
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using link_type = list_node*;

        link_type node;
        // constructor
        _list_node_iterator(): node(nullptr){};

        _list_node_iterator(link_type x): node(x){}

        _list_node_iterator(const self& x): node(x.node){}

        _list_node_iterator(self&& x): node(x.node) { x.node = nullptr; }
        // operator
        _list_node_iterator& operator=(const self& x)
        {
            node = x.node;
            return *this;
        }
        bool operator==(const self& x) const { return node == x.node; }
        bool operator!=(const self& x) const { return node != x.node; }
        reference operator*() const { return (*node).data; }
        pointer operator->() const { return &(node->data); }

        self& operator++()
        {
            node = node->next;
            return *this;
        }

        self& operator++(int)
        {
            self tmp = *this;
            node = node->next;
            return tmp;
        }

        self& operator--()
        {
            node = node->prev;
            return *this;
        }

        self& operator--(int)
        {
            self tmp = *this;
            node = node->prev;
            return tmp;
        }
    };

    // 环形链表
    template <class T, class Alloc = allocator<T>>
    class list
    {
    protected:
        using data_allocator = Alloc;
        using node_allocator = allocator<_list_node<T>>;
        using list_node = _list_node<T>;
        using iterator = _list_node_iterator<T, T&, T*>;
        using const_iterator = _list_node_iterator<T, const T&, const T*>;
        using reverse_iterator = awstl::reverse_iterator<iterator>;
        using const_reverse_iterator = awstl::reverse_iterator<const_iterator>;
        using size_type = typename _list_node_iterator<T, T&, T*>::size_type;
        using difference_type = typename _list_node_iterator<T, T&, T*>::difference_type;
        using reference = T&;
        using vector_iterator = T*;

        list_node* _node; // 指向尾部的指针,头部 prev，尾部next
        data_allocator alloc;
        node_allocator node_alloc;
        size_type _size;

    protected:
        void empty_initialize()
        {
            _node = node_allocator::allocate();
            _node->unlink();
        }

        void _fill_init(size_type n, const T& val);

        template <class... Args>
        static list_node* create_Anode(Args&&... args)
        {
            list_node* Anode = node_allocator::allocate();
            if (Anode == nullptr)
                return nullptr;
            node_allocator::construct(Anode, awstl::forward<Args>(args)...);
            return Anode;
        }

        void _check_size()
        {
            auto _curr = distance(begin(), end());
            _size = _curr == _size ? _size : _curr; // _size = _curr
        }
        void transfer(iterator position, iterator first, iterator last);
    public:
        iterator begin() { return iterator(_node->next); } // 返回指向第一个元素的迭代器,显式构造
        iterator end() { return iterator(_node); } // 返回指向尾部的迭代器,显式构造
        const_iterator cbegin() const { return const_iterator(_node->next); }
        const_iterator cend() const { return const_iterator(_node); }
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator crbegin() const { return reverse_iterator(cend()); }
        const_reverse_iterator crend() const { return reverse_iterator(cbegin()); }
#pragma region operation

    public:
        list& operator=(const list& other)
        {
            if (this == &other)
                return *this;
            clear();
            for (auto x = other.begin(); x != other.end(); ++x)
            {
                push_back(*x);
            }
            return *this;
        }
        list& operator=(list&& other) noexcept
        {
            if (this == &other)
                return *this;
            clear();
            _node = other._node;
            alloc = other.alloc;
            _size = other._size;
            return *this;
        }
        
        bool empty() noexcept
        {
            _check_size();
            return _size == 0;
        }

        constexpr size_type size() noexcept { _check_size(); return _size; }
        reference front() { return *begin(); }
        reference back() { return *(--end()); }
        void push_back(const T& x);
        void emplace_back(T&& x);
        void push_back(T&& x) { emplace_back(move(x)); }
        void push_front(const T& x);
        void emplace_front(T&& x);
        void push_front(T&& x) { emplace_front(move(x)); }
        void pop_back();
        void pop_front();
        void clear();
        iterator erase(iterator position);
        void remove(const T& value);
        iterator insert(iterator position, const T& x);
        iterator insert(iterator position, T&& x);
        iterator find(iterator start, iterator end, const T& value);

        void splice(iterator position, list& x);
        void splice(iterator position, list&, iterator i);
        iterator splice(iterator position, list&, iterator first, iterator last);
        iterator merge(list& x);
        void swap(list& x) noexcept ;
        
        void reverse();
        void sort();
#pragma endregion

#pragma region constructor

    public:
        list(): _node(),_size(0) { empty_initialize(); }

        list(const list& other): _node(other._node), alloc(other.alloc),node_alloc(other.node_alloc), _size(other._size)
        {
            for (auto x = other.cbegin(); x != other.cend(); ++x)
            {
                push_back(*x);
            }
        };

        list(list&& other) noexcept : _node(other._node), alloc(other.alloc), node_alloc(other.node_alloc), _size(other._size) { other->~awlist(); }

        list(size_type n, const T& value): alloc(data_allocator()), node_alloc(node_allocator())
        {
            _fill_init(n, value);
            _size = n;
        }

        list(size_type n): list(n, T()){}
        
        list(iterator begin, iterator finish)
        {
            _node = create_Anode();
            if (_node == nullptr)
            {
                std::cerr << "create_Anode failed" << std::endl;
                return;
            }
            try
            {
                _node->unlink();
                auto ptr_origin = begin;
                while(ptr_origin != finish)
                {
                    auto tmp = create_Anode(*ptr_origin);
                    emplace_back(move(tmp));
                    ++ptr_origin;
                }
                _size = distance(begin, finish);
            }
            catch (...)
            {
                std::cerr << "construct failed" << std::endl;
                clear();
            }
        }

        template <class InputIter>
        list(InputIter begin, InputIter finish): node_alloc(node_allocator())
        {
            _node = create_Anode();
            if (_node == nullptr)
            {
                std::cerr << "create_Anode failed" << std::endl;
                return;
            }
            try
            {
                _node->unlink();
                auto ptr_origin = begin;
                while(ptr_origin != finish)
                {
                    auto tmp = create_Anode(*ptr_origin);
                    push_back((*tmp).data);
                    ++ptr_origin;
                }
            }
            catch (...)
            {
                clear();
            }
        }
        
#pragma endregion

    public:
        ~list()
        {
            if (!empty())
            {
                for (auto x = this->begin(); x != this->end();)
                {
                    node_allocator::destroy(x.node);
                    auto tmp = x.node;
                    ++x;
                    node_allocator::deallocate(tmp);
                }
            }
            node_allocator::destroy(_node);
        }
    };

    template <class T, class Alloc = allocator<T>>
    void list<T, Alloc>::_fill_init(size_type n, const T& val)
    {
        // 初始化，此时 _node 没有指向对象
        this->_node = create_Anode();
        if (_node == nullptr)
        {
            std::cerr << "create_Anode failed" << std::endl;
            return;
        }
        try
        {
            _node->unlink();
            list_node* tmp = nullptr;
            for (size_type i = 0; i < n; ++i)
            {
                tmp = create_Anode(val);
                tmp->next = _node;
                tmp->prev = _node->prev;
                tmp->prev->next = tmp;
                _node->prev = tmp;
            }
        }
        catch (...)
        {
            auto destroy_ptr = _node->next;
            while (destroy_ptr != _node)
            {
                auto tmp = destroy_ptr;
                destroy_ptr = destroy_ptr->next;
                alloc.destroy(tmp);
                alloc.deallocate(tmp);
            }
            alloc.destroy(_node);
            alloc.deallocate(_node);
        }
    }

    template <class T, class Alloc>
    void list<T, Alloc>::push_back(const T& x)
    {
        list_node* tmp = create_Anode(x);
        tmp->next = _node;
        tmp->prev = _node->prev;
        tmp->prev->next = tmp;
        _node->prev = tmp;
        ++_size;
    }

    template <class T, class Alloc>
    void list<T, Alloc>::emplace_back(T&& x)
    {
        list_node* tmp = create_Anode(awstl::move(x));
        tmp->next = _node;
        tmp->prev = _node->prev;
        tmp->prev->next = tmp;
        _node->prev = tmp;
        ++_size;
    }

    template <class T, class Alloc>
    void list<T, Alloc>::push_front(const T& x)
    {
        list_node* tmp = create_Anode(x);
        tmp->next = _node->next;
        tmp->prev = _node;
        tmp->next->prev = tmp;
        _node->next = tmp;
        ++_size;
    }

    template <class T, class Alloc>
    void list<T, Alloc>::emplace_front(T&& x)
    {
        list_node* tmp = create_Anode(awstl::move(x));
        tmp->next = _node->next;
        tmp->prev = _node;
        tmp->prev->next = tmp;
        tmp->next->prev = tmp;
        ++_size;
    }


    template <class T, class Alloc>
    void list<T, Alloc>::pop_back()
    {
        if (empty())
            return;
        erase(--end());
    }

    template <class T, class Alloc>
    void list<T, Alloc>::pop_front()
    {
        if(empty())
            return;
        erase(begin());
    }

    template <class T, class Alloc>
    void list<T, Alloc>::clear()
    {
        if (empty())
            return;
        auto destroy_ptr = _node->next;
        while (destroy_ptr != _node)
        {
            auto tmp = destroy_ptr;
            destroy_ptr = destroy_ptr->next;
            node_alloc.destroy(tmp);
            node_alloc.deallocate(tmp);
        }
        _node->unlink();
        _size = 0;
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator position)
    {
        if (empty())
            return;
        auto ptr = begin();
        auto last = end();
        while (ptr != last)
        {
            if (&(*ptr) == &(*position))
            {
                auto tmp = ptr.node;
                ++ptr;
                ptr.node->prev->next = ptr.node->next;
                ptr.node->next->prev = ptr.node->prev;
                alloc.destroy(tmp);
                alloc.deallocate(tmp);
                --_size;
                return ptr;
            }
            else
                ++ptr;
        }
        return iterator(nullptr); // awlist<T,Alloc>::iterator(nullptr);  
    }

    template <class T, class Alloc>
    void list<T, Alloc>::remove(const T& value)
    {
        if (empty())
            return;
        auto ptr = begin();
        auto last = end();
        while (ptr != last)
        {
            if (*ptr == value)
            {
                auto tmp = ptr;
                ++ptr;
                erase(tmp);
            }
            else
                ++ptr;
        }
    }


    template <class T, class Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator position, const T& x)
    {
        //todo
        list_node* tmp = create_Anode(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        tmp->prev->next = tmp;
        position.node->prev = tmp;
        ++_size;
        return iterator(tmp);
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator position, T&& x)
    {
        list_node* tmp = create_Anode(awstl::move(x));
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        tmp->prev->next = tmp;
        position.node->prev = tmp;
        ++_size;
        return iterator(tmp);
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::find(iterator start, iterator end, const T& value)
    {
        if (empty())
            return iterator(nullptr);
        auto ptr = start;
        while (ptr != end)
        {
            if (*ptr == value)
                return ptr;
            ++ptr;
        }
        return iterator(nullptr);
    }

    template <class T, class Alloc>
    void list<T, Alloc>::splice(iterator position, list& x)
    {
        if (!x.empty())
        {
            transfer(position, x.begin(), x.end());
            _size += x.size();
            x._size = 0;
        }
    }

    template <class T, class Alloc>
    void list<T, Alloc>::splice(iterator position, list&, iterator i)
    {
        auto j = i;
        ++j;
        if(position == i || position == j)
            return ;
        transfer(position,i, j);
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::splice(iterator position, list&, iterator first, iterator last)
    {
        if(position == first || position == last)
            return position;
        return transfer(position, first, last);
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator list<T, Alloc>::merge(list<T, Alloc>& x)
    {
        if (this == &x)
            return begin();
        auto ptr_this = begin();
        auto ptr_x = x.begin();
        while(ptr_this != end() && ptr_x != x.end())
        {
            if(*ptr_x < *ptr_this)
            {
                auto tmp = ptr_x;
                transfer(ptr_this, ptr_x, ++tmp);
                ptr_x = tmp;
            }
            else
            {
                ++ptr_this;
            }
        }
        if(ptr_x != x.end())
        {
            transfer(end(), ptr_x, x.end());
        }
        _size += x.size();
        x._size = 0;
        x._node->unlink();
        return begin();
    }

    template <class T, class Alloc>
    void list<T, Alloc>::swap(list& x) noexcept
    {
        auto tmp_node = this->_node;
        this->_node = x._node;
        x._node = tmp_node;
        auto tmp = x._size;
        x._size = _size;
        _size = tmp;
    }

    template <class T, class Alloc>
    void list<T, Alloc>::reverse()
    {
        if(empty() || size() == 1)
            return;
        auto target = begin();
        ++target;
        while(target != end())
        {
            auto tmp = target;
            ++target;
            transfer(begin(), tmp, target);
        }
    }

    template <class T, class Alloc>
    void list<T, Alloc>::sort()
    {
        if(empty() || _node->next->next == _node)
            return ;
        list<T, Alloc> carry;
        list<T, Alloc> counter[64];
        int fill = 0;
        while(!empty())
        {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while(i < fill && ! counter[i].empty())
            {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if(i == fill) ++fill;
        }
        for(int i = 1 ; i < fill ; ++i)
            counter[i].merge(counter[i-1]);
        swap(counter[fill - 1]);
    }

    template <class T, class Alloc>
    void list<T, Alloc>::transfer(iterator position, iterator first, iterator last)
    {
        if(position != last)
        {
            last.node->prev->next = static_cast<list_node*>(position.node);
            first.node->prev->next = static_cast<list_node*>(last.node);
            auto tmp = position.node->prev;
            tmp->next = static_cast<list_node*>(first.node);
            position.node->prev = static_cast<list_node*>(last.node->prev);
            last.node->prev = static_cast<list_node*>(first.node->prev);
            first.node->prev = static_cast<list_node*>(tmp);
        }
    }

    
}


#endif
