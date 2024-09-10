#ifndef _SET_HPP_
#define _SET_HPP_

#include "rb_tree.hpp"
#include  "functional"

namespace awstl
{
    
    template <class Key, class Compare = std::less<Key>, class Alloc = awstl::allocator<_rb_tree_node<Key>>>
    class set
    {
    public:
        //typedef
        using key_type = Key;
        using value_type = Key;
        using key_compare = Compare;
        using value_compare = Compare;
   
    private:
        using rep_type = rb_tree<key_type, value_type, identity<Key>, key_compare, Alloc>;
        rep_type _rb_tree;
    public:
        using pointer = typename rep_type::pointer;
        using const_pointer = typename rep_type::const_pointer;
        using reference = typename rep_type::reference;
        using iterator = typename rep_type::const_iterator; // set的迭代器是const的
        using size_type = typename rep_type::size_type;

#pragma region ctor
        set(): _rb_tree(rep_type()) {}
        template <class InputIterator>
        set(InputIterator first, InputIterator last):_rb_tree(rep_type())
        {
            _rb_tree.insert_unique(first, last);
        }
        
#pragma endregion

#pragma region operation
        set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x)
        {
            _rb_tree = x._rb_tree;
            return *this;
        }

        bool operator==(set<Key, Compare, Alloc>&& x)
        {
            return &x._rb_tree == &_rb_tree;
        }

        bool operator!=(set<Key, Compare, Alloc>&& x)
        {
            return &x._rb_tree != &_rb_tree;
        }
#pragma endregion

    public:
        key_compare key_comp() const { return _rb_tree.key_compare(); }
        value_compare value_comp() const { return _rb_tree.key_compare(); }
        iterator begin() const { return _rb_tree.cbegin(); }
        iterator end() const { return _rb_tree.cend(); }
        bool empty() const { return _rb_tree.empty(); }
        size_type size() const { return _rb_tree.size(); }

        //insert / erase
        std::pair<iterator, bool> insert(const value_type& x)
        {
            auto p = _rb_tree.insert_unique(x);
            return std::pair<iterator, bool>(iterator(p.first) , p.second);
        }

        bool erase(const value_type& x)
        {
            return  _rb_tree.erase(_rb_tree.find(x));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            _rb_tree.insert_unique(first, last);
        }

        void clear() {return _rb_tree.clear();}
        iterator find(const key_type& x) {return _rb_tree.find(x);}

        
        
    };
    
}


#endif  