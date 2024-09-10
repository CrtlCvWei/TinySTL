#ifndef _RB_TREE_HPP_
#define _RB_TREE_HPP_

#include <queue>
#include <stack>

#include "allocator.hpp"
#include "iterator.hpp"
#include "memory.hpp"

namespace awstl
{
#define has_no_children(n) (n->left == nullptr && n->right == nullptr)
#define has_one_child(n) (n->left == nullptr && n->right != nullptr) || (n->left != nullptr && n->right == nullptr)
#define child_color(n) (n->left ? n->left->color_ : n->right->color_)
#define setcolor(n, c) n->color_ = c

    enum class RBColor
    {
        Black = 0,
        Red = 1,

        
    };

    std::ostream& operator << (std::ostream& os, const RBColor& color)
    {
        if(color == RBColor::Black)
            os << "Black";
        else
            os << "Red";
        return os;
    }
    
    struct _rb_tree_node_base
    {
        using color = RBColor;
        using base_ptr = _rb_tree_node_base*;
        
        color color_;
        base_ptr parent;
        base_ptr left;
        base_ptr right;

        static base_ptr maximum(base_ptr x)
        {
            while(x->right) x = x->right;
            return x;
        }

        static  base_ptr minimum(base_ptr x)
        {
            while(x->left) x = x->left;
            return x;
        }
    };

    template <class T>
    struct _rb_tree_node: public  _rb_tree_node_base
    {
        using link_type = _rb_tree_node<T>*;
        using value_type = T;
        T value_field;

        _rb_tree_node(const T& x): value_field(x) {}
    };

    struct _rb_tree_base_iterator
    {
        using base_ptr = _rb_tree_node_base::base_ptr;
        using iterator_category = awstl::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = _rb_tree_base_iterator;

        base_ptr node;

        _rb_tree_base_iterator() = default;
        _rb_tree_base_iterator(base_ptr x): node(x) {}
        _rb_tree_base_iterator(const self& other):node(other.node) {}
        _rb_tree_base_iterator(self&& other) noexcept: node(other.node) {}
        void increment();
        void decrement();
    };

    /* _rb_tree_base_iterator movement */
    inline void _rb_tree_base_iterator::increment()
    {
        auto ptr = node;
        if(ptr->right)
        {
            ptr = ptr->right;
            while(ptr->left) ptr = ptr->left;
        }
        else
        {
            // 找到第一个有
            auto y = ptr->parent;
            while(y && y->right == ptr)
            {
                ptr = y;
                y = ptr->parent;
            }
            if(y ->right != ptr)
                ptr = y;
        }
        node = ptr;
    }
    inline void _rb_tree_base_iterator::decrement()
    {
        auto ptr = node;
        // @brief: awstl::RB Tree  header , that is end()
        if(RBColor::Red == ptr->color_ && ptr->parent->parent == ptr)
        {
            node = ptr->right;
            return;
        }
        if(ptr->left)
        {
            ptr = ptr->left;
            ptr = _rb_tree_node_base::maximum(ptr);
        }
        else
        {
            auto y = ptr->parent;
            while(y && y->left == ptr)
            {
                ptr = y;
                y = ptr->parent;
            }
            ptr = y;
        }
        node = ptr;
    }

#pragma region balance

    // x 为旋转点, 这一步没有染色, y 为 新的支点
    inline void __rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base* &root)
    {
        auto y = x->right;
        x->right = y->left;
        if(y->left != nullptr)
            y->left->parent = x;
        
        y->parent = x->parent;
        if(x == root)
            root = y;
        else if(x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    // x 为旋转点, 这一步没有染色
    inline void __rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base* &root)
    {
        auto y = x->left;
        x->left = y->right;
        if(y->right) // safe nagivation
            y->right->parent = x;
        y->parent = x->parent;
        if(root == x)
            root = y;
        else if(x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->right = x;
        x->parent = y;
    }

    // x 为新插入的节点， root为根节点，插入后将调整平衡并染色
    inline void __rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base* &root)
    {
        setcolor(x, RBColor::Red);
        _rb_tree_node_base* uncle;
        while(x != root && x->parent->color_ == RBColor::Red)
        {
            // Father is red so we need to adjust the tree
            if(x->parent == x->parent->parent->left) // Father is left child
            {
                uncle = x->parent->parent->right;
                if(uncle && uncle->color_ == RBColor::Red)
                {
                    x->parent->color_ = uncle->color_ = RBColor::Black;
                    x->parent->parent->color_ = RBColor::Red;
                    x = x->parent->parent;
                }
                else
                {   // NULLPTR is Black!
                    if(x == x->parent->right)
                    {
                        __rb_tree_rotate_left(x->parent, root);
                        x = x->left;
                    }
                     // NOW is LL and x has been changed to x->parent
                    x->parent->color_ = RBColor::Black;
                    x = x->parent->parent;
                    x->color_ = RBColor::Red;
                    __rb_tree_rotate_right(x, root);
                }
            }
            else
            {
                // Father is right child
                uncle = x->parent->parent->left;
                if(uncle && uncle->color_ == RBColor::Red)
                {
                    setcolor(x->parent, RBColor::Black);setcolor(uncle, RBColor::Black);
                    setcolor(x->parent->parent, RBColor::Red);
                    x = x->parent->parent; // continue to adjust
                }
                else
                {
                    if(x == x->parent->left)
                    {
                        __rb_tree_rotate_right(x->parent, root);
                        x = x->right;
                    }
                    setcolor(x->parent, RBColor::Black);
                    x = x->parent->parent;
                    setcolor(x, RBColor::Red);
                    __rb_tree_rotate_left(x, root);
                }
            }
        } // end while
        setcolor(root, RBColor::Black); // root is always black
    }
#pragma endregion
    
    // iterator
    template <class T, class Ref, class Ptr>
    struct _rb_tree_iterator: public _rb_tree_base_iterator
    {
        using value_type = T;
        using reference = Ref;
        using pointer = Ptr;
        using link_type = _rb_tree_node<T>*;
        using self = _rb_tree_iterator<T, Ref, Ptr>;
        _rb_tree_iterator() = default;
        _rb_tree_iterator(link_type x) { node = x;};
        _rb_tree_iterator(const _rb_tree_iterator& it): _rb_tree_base_iterator(it.node) {}
        _rb_tree_iterator(_rb_tree_iterator&& it) noexcept: _rb_tree_base_iterator(std::forward<_rb_tree_base_iterator>(it)) {}

        // 允许从不同的迭代器类型转换
        template <class OtherPtr>
        _rb_tree_iterator(const _rb_tree_iterator<T, Ref, OtherPtr>& other)
            : _rb_tree_base_iterator(other.node) {}
        reference operator*() const
        {
            return static_cast<reference>(
                static_cast<link_type>(node)->value_field
            );
        }

        reference operator->() const{return &(operator*());}
        self& operator++()
        {
            increment();
            return *this;
        }
        self operator++(int)
        {
            auto temp = *this;
            increment();
            return temp;
        }
        self& operator--()
        {
            decrement();
            return *this;
        }
        self operator--(int)
        {
            auto temp = *this;
            decrement();
            return temp;
        }

        self& operator=(const self& it)
        {
            node = it.node;
            return *this;
        }
        
        bool operator==(const self& it) const { return node == it.node; }
        bool operator!=(const self& it) const { return node != it.node; }
        
    };

    template <class T, class Ref, class Ptr>
    struct  _rb_tree_iterator<T, const Ref, const Ptr>: public _rb_tree_base_iterator
    {
        using value_type = T;
        using reference = const Ref;
        using pointer = const Ptr;
        using link_type = _rb_tree_node<T>*;
        using self = _rb_tree_iterator<T,const Ref,const Ptr>;
        using non_const_iterator = _rb_tree_iterator<T, Ref, Ptr>;

        _rb_tree_iterator() = default;
        ~_rb_tree_iterator() = default;
        _rb_tree_iterator(link_type x) { node = x;};
        _rb_tree_iterator(const _rb_tree_iterator& it): _rb_tree_base_iterator(it.node) {}
        _rb_tree_iterator(_rb_tree_iterator&& it) noexcept: _rb_tree_base_iterator(std::forward<_rb_tree_base_iterator>(it)) {}
        _rb_tree_iterator(const non_const_iterator& it): _rb_tree_base_iterator(it.node) {}
        
        const reference operator*() const
        {
            return static_cast<reference>(
                static_cast<link_type>(node)->value_field
            );
        }

        const reference operator->() const{return const_cast<const reference>(&(operator*()));}

        const self& operator++()
        {
            increment();
            return const_cast<const self&>( *this);
        }
        self operator++(int)
        {
            auto temp = *this;
            increment();
            return temp;
        }
        const self& operator--()
        {
            decrement();
            return *this;
        }
        self operator--(int)
        {
            auto temp = *this;
            decrement();
            return temp;
        }

        const self& operator=(const self& it)
        {
            node = it.node;
            return *this;
        }
        
        bool operator==(const self& it) const { return node == it.node; }
        bool operator!=(const self& it) const { return node != it.node; }
    };
    
    // 红黑树
    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = awstl::allocator<_rb_tree_node<Value>>>
    class rb_tree
    {
    protected:
        using void_pointer = void*;
        using base_ptr = _rb_tree_node_base*;
        using base_node = _rb_tree_node_base;
        using rb_tree_node_alloc = Alloc;
        using color = RBColor;
    public:
        using key_type = Key;
        using value_type = Value;
        using key_compare = Compare;
        using reference = Value&;
        using const_reference = const Value&;
        using pointer = Value*;
        using const_pointer = const Value*; // pointer to const Value
        using link_type = _rb_tree_node<Value>*;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using node = _rb_tree_node<Value>;
        using iterator = _rb_tree_iterator<value_type, reference, pointer>;
        using const_iterator = _rb_tree_iterator<value_type, reference, const Value*>;
    protected:
        link_type get_node(){ return rb_tree_node_alloc::allocate(1); }
        void put_node(link_type x) { rb_tree_node_alloc::deallocate(x); }

        link_type create_node(Value x)
        {
            auto tmp = get_node();
            try
            {
                awstl::construct(tmp, node(x));
                tmp->parent = tmp->left = tmp->right = nullptr;
                tmp->color_ = color::Red; // default color of a new node, that's the rule
            }
            catch (...)
            {
                put_node(tmp);
                throw;
            }
            return tmp;
        }
        link_type clone_node(const link_type& x)
        {
            auto _node = get_node();
            try
            {
                awstl::construct(_node, *x);
                _node->color_ = x->color_;
                _node->left = _node->parent = _node->right = nullptr;
            }
            catch (...)
            {
                put_node(_node);
                throw;
            }
            return _node;
        }
        void destroy_node(link_type p)
        {
            awstl::destroy(&p->value_field);
            put_node(p);
            --node_count;
        }
    protected:
        size_type node_count = 0;
        link_type header; // header node
        key_compare compare_rule;

        link_type& root() const { return reinterpret_cast<link_type&>(header->parent); }
        link_type& leftmost() const { return reinterpret_cast<link_type&>(header->left); }
        link_type& rightmost() const { return reinterpret_cast<link_type&>(header->right); }

        // 获取节点的成员
        static inline  link_type& left(link_type& x)  { return reinterpret_cast<link_type&>(x->left); }
        static inline  link_type& right(link_type& x)  { return reinterpret_cast<link_type&>(x->right); }
        static inline  link_type& parent(link_type& x)  { return reinterpret_cast<link_type&>(x->parent); }
        static link_type& grandparent(link_type& x)  { return x->parent ? parent(x->parent) : nullptr; }
        static reference value(link_type& x) { return x->value_field; }
        static key_type& key(base_ptr& x) { return KeyOfValue()(value(x)); }
        static color& _color(base_ptr& x) { return x->color_; }
        static color& _color(link_type& x) { return _color(reinterpret_cast<base_ptr&>(x)); }
        static key_type& key(link_type& x) { return key(reinterpret_cast<base_ptr&>(x)); }
        static reference value(base_ptr& x) { auto y = reinterpret_cast<link_type&>(x); return y->value_field; }

        // 封装
        static link_type minimum(link_type& x)
        {
            return static_cast<link_type> (base_node::minimum(x));
        }
        static link_type maximum(link_type& x)
        {
            return static_cast<link_type> (base_node::maximum(x));
        }

        bool erase_aux(link_type x);
    public:

        
        bool empty() const { return node_count == 0; }
        iterator begin() {return leftmost();}
        iterator end() {return header;}
        iterator max() {return rightmost();}
        size_t size() const {return node_count;}
        const_iterator cbegin () const {return const_iterator(leftmost());}
        const_iterator cend() const {return  const_iterator(header);}
        Compare key_comp() const { return compare_rule; }
        bool erase(iterator it) { return erase_aux(reinterpret_cast<link_type>(it.node)); }
        iterator find(const key_type& x);
       
        void clear();
    private:
        iterator _insert(base_ptr _x, base_ptr _p, const value_type& v);
        void init()
        {
            node_count = 0;
            header = get_node();
            _color(header) = color::Red;
            leftmost()  = rightmost() = header;
            header->parent = nullptr;
        }
    
    public:
        rb_tree():compare_rule(Compare()) {init();}
        rb_tree(const key_compare& compare): compare_rule(compare) {init();}
        ~rb_tree() = default;
        bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&);
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& other);
        
        // most important
        std::pair<iterator,bool> insert_unique(const value_type& x);
        std::pair<iterator,bool> insert_equal(const value_type& x);
        template<class InputIterator>
        void insert_unique(InputIterator first, InputIterator last)
        {
            for(; first != last; ++first)
                insert_unique(*first);
        };
        template<class InputIterator>
        void insert_equal(InputIterator first, InputIterator last)
        {
            for(; first != last; ++first)
                insert_equal(*first);
        };
        color Color(iterator it) { return _color(it.node); }
        iterator getroot() const { return iterator(root()); }
        static iterator getleft(iterator it)
        {
            auto node = reinterpret_cast<link_type>(it.node);
            return iterator(left( node));
        }
        static iterator getright(iterator it)
        {
            auto node = reinterpret_cast<link_type>(it.node);
            return iterator(right(node));
        }
    };

    // 删除节点，返回是否删除成功
    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    bool rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase_aux(link_type x)
    {
        if(x == nullptr)
        {
            std::cerr << "Error:  erase null node in rb_tree" << std::endl;
            return false;
        }
        if(has_no_children(x))
        {
            auto p = parent(x);
            auto father_color = _color(p);
            if(_color(x) == RBColor::Red)
            {
                p->left == x ? p->left = nullptr : p->right = nullptr;
                destroy_node(x);
                return true;
            }
            else
            {
                //TODO x 为黑色节点
                auto brother = p->left == x ? p->right : p->left;
                if(brother && _color(brother) == RBColor::Red)
                {
                    if(father_color == RBColor::Red)
                    {
                        std::cerr << "Error:  the rb tree against the rule!" << std::endl;
                        return false;
                    }
                    if(brother == p->left)
                    {
                        __rb_tree_rotate_right(p, *reinterpret_cast<_rb_tree_node_base**>(root()));
                    }
                    else
                    {
                        __rb_tree_rotate_left(p, *reinterpret_cast<_rb_tree_node_base**>(root()));
                    }
                    setcolor(brother, RBColor::Black);
                    setcolor(p, RBColor::Red);
                    return erase(x);
                }
                else
                {
                    // if brother is nullptr or black
                    if(!brother)
                    {
                        if(father_color == RBColor::Red)
                        {
                            setcolor(p, RBColor::Black);
                        }
                        else
                        {
                            std::cerr << "Error:  the rb tree against the rule!" << std::endl;
                            return false;
                        }
                    }
                    else
                    {
                        if(has_one_child(brother))
                        {
                            if(brother == p->right)
                            {
                                auto bro_son = brother->left? brother->left : brother->right;
                                if(bro_son == brother->left)
                                {
                                    __rb_tree_rotate_right(brother, *reinterpret_cast<_rb_tree_node_base**>(root()));
                                    setcolor(brother, RBColor::Red);setcolor(bro_son, RBColor::Black);
                                    brother = bro_son;
                                }
                                __rb_tree_rotate_left(p,*reinterpret_cast<_rb_tree_node_base**>(root()));
                            }
                            else
                            {
                                auto bro_son = brother->left? brother->left : brother->right;
                                if(bro_son == brother->right)
                                {
                                    __rb_tree_rotate_left(brother, *reinterpret_cast<_rb_tree_node_base**>(root()));
                                    setcolor(brother, RBColor::Red);setcolor(bro_son, RBColor::Black);
                                    brother = bro_son;
                                }
                                __rb_tree_rotate_right(p,*reinterpret_cast<_rb_tree_node_base**>(root()));
                            }
                            // set color
                            setcolor(brother, father_color);
                            setcolor(p, RBColor::Black);
                            setcolor(brother->right, RBColor::Black);
                        }
                        else if(has_no_children(brother))
                        {
                            if(father_color == RBColor::Red)
                            {
                                setcolor(p, RBColor::Black);setcolor(brother, RBColor::Red);
                            }
                            else
                            {
                                //TODO
                            }
                        }
                    }
                    x == p->left ? p->left = nullptr : p->right = nullptr;
                    destroy_node(x);
                    return true;
                }
                
                
            }
        }
        else if(has_one_child(x))
        {
            auto p = x->parent;
            auto child = x->left ? x->left : x->right;
            if(_color(x) == RBColor::Red)
            {
                std::cerr << "Error:  erase red node with one child in rb_tree" << std::endl;
                return false;
            }
            else
            {
                x == p->left ? p->left = child : p->right = child;
                child->parent = p;
                setcolor(child, RBColor::Black);
                destroy_node(x);
                return true;
            }
        }
        else
        {
            //TODO 有两个孩子的情况，后继节点默认为右子树的最小节点
            auto node = reinterpret_cast<link_type>(x->right);
            auto successor = minimum(node);
            x->value_field = successor->value_field;
            return erase(successor);
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    find(const key_type& x)
    {
        auto j = root();
        while(j != nullptr)
        {
            if(compare_rule(x, key(j))) //  true when x < key(j.node)
            {
                j = left(j);
            }
            else
            {
                if(compare_rule(key(j), x)) // true when key(j.node) < x
                    j = right(j);
                else
                    return iterator(j);
            }
        }
        return end();
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::clear()
    {
        auto it = root();
        std::stack<link_type> stk;
        link_type ban = nullptr;
        while(!stk.empty() || it)
        {
            if(it)
            {
                stk.emplace(it);
                it = left(it);
            }
            else
            {
                it = stk.top();
                if(ban != it && right(it))
                {
                    it = right(it);
                }
                else
                {
                    ban = it;
                    stk.pop();
                    auto tmp = it;
                    auto p = parent(it);
                    if(p != header)
                    {
                        it == left(p) ? left(p) = nullptr : right(p) = nullptr;
                    }
                    else
                    {
                        root() = nullptr;
                        leftmost() = rightmost() = header;
                    }
                    it = nullptr;
                    destroy_node(tmp);

                }
            }
        }
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    _insert(base_ptr _x, base_ptr _p, const value_type& v)
    {
        link_type x = static_cast<link_type> (_x);
        link_type p = static_cast<link_type> (_p);
        link_type z;

        if(p == header || x != nullptr || compare_rule(KeyOfValue()(v), key(p)))//  comp is true when key(v) < key(p)
        {
            z = create_node(v);
            left(p) = z;
            if(p == header)
            {
                parent(z) = header;
                root() = z;
                if(node_count == 0)
                    left(header) = right(header) = z;
            }
            else if(p == leftmost())
            {
                leftmost() = z;
            }
        }
        else
        {
            z = create_node(v);
            right(p) = z;
            if(p == rightmost())
            {
                rightmost() = z;
            }
        }

        parent(z) = p;
        left(z) = right(z) = nullptr;
        __rb_tree_rebalance(z, header->parent);
        ++node_count;
        return iterator(z);
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    bool rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::operator==(
        const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&)
    {
        //TODO
        return false;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::operator=(
        const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& other)
    {
        this->root() = other.root();
        this->leftmost() = other.leftmost();
        this->rightmost() = other.rightmost();
        this->header = other.header;
        // some state value
        this->node_count = other.node_count;
        this->compare_rule = other.compare_rule;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool> rb_tree<Key, Value, KeyOfValue,
    Compare, Alloc>::insert_unique(const value_type& x)
    {
        link_type p = header;
        link_type ptr = root();
        auto v = x ;
        bool comp = true;
        while(ptr)
        {
            p = ptr;
            // comp 为 true 时，表示  key(x) < key(ptr)
            comp = compare_rule(KeyOfValue()(v), key(ptr));
            if(comp)
                ptr = left(ptr);
            else
                ptr = right(ptr);
        }
        iterator j = iterator(p);
        if(comp)
            if(j == begin())
                return std::make_pair(_insert(ptr, p, v), true);
            else --j;
            
        if(compare_rule(key(j.node),KeyOfValue()(v)))
            return std::make_pair(_insert(ptr, p, v), true);
        return std::make_pair(j, false);
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool> rb_tree<Key, Value, KeyOfValue,
    Compare, Alloc>::insert_equal(const value_type& x)
    {
        link_type p = header;
        link_type ptr = root();
        bool comp = true;
        while(ptr)
        {
            p = ptr;
            // comp 为 true 时，表示  key(x) < key(ptr)
            comp = compare_rule(KeyOfValue()(x), key(ptr));
            if(comp)
                ptr = left(ptr);
            else
            {
                if(key(ptr) == KeyOfValue()(x))
                {
                    ptr->value_field = x;
                    return std::make_pair(iterator(ptr), true);
                }
                ptr = right(ptr);
            }
        }
        return std::make_pair(__insert(ptr, p, x), true);
    }
}

#endif
