#ifndef _LUA_TABLE_H_
#define _LUA_TABLE_H_
#include <unordered_map>

#include "allocator.hpp"
#include "vector.hpp"

/*
 * @Author: Aw
 * @Date: 2024-09-09 20:30:00
 * @Brief: This file is used to define the lua table_key struct.
*/

namespace awstl
{
#define NIL NULL
    
    template <class T>
    union table_key
    {
        struct 
        {
            T value;
            int next;
        } nk;
        int tvk;
    };

    template <class T>
    struct table_Node
    {
        table_key<T> i_key; // key
        T i_val; // value
    };
    
    template <class T>
    using TKey = table_key<T>;
    
    template <class T>
    using TNode = table_Node<T>;
    
    // template <class T>
    
    
    
    template <class T,class TSquence = awstl::vector<T>,class Alloc_Node = awstl::allocator<TNode<T>>,class Alloc = awstl::allocator<TKey<T>>>
    class lua_table
    {
    public:
        using node = TNode<T>;
        using key = TKey<T>;
        using node_alloc = Alloc_Node;
        using key_alloc = Alloc;
        using Tseq = TSquence;
        using Hseq = std::unordered_map<T>; // hash table
        using size_type = size_t;
        using iterator_t = typename Tseq::iterator;
        using iterator_n = typename Hseq::iterator;
    protected:
        size_type _size_array;
        size_type _size_node;

        iterator_t _arrary; // pointer to the array part
        iterator_n _node; // pointer to the node part
    };
    
}




#endif
