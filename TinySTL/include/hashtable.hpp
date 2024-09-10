#ifndef _HASH_TABLE_HPP_
#define _HASH_TABLE_HPP_

#include <cstddef>
#include <vector>

#include "allocator.hpp"
#include "iterator.hpp"
#include "vector.hpp"

namespace awstl
{
    class hashtable;
    
    template <class T>
    struct _hashtable_node
    {
        _hashtable_node* next;
        T value;

        _hashtable_node(const T& v, _hashtable_node* n) : next(n),value(v) {}
        _hashtable_node(T&& v, _hashtable_node* n) : next(n),value(std::move(v)) {}
        _hashtable_node(const T& v): next(nullptr),value(v) {}
        _hashtable_node(T&& v) noexcept : next(nullptr) , value(std::move(v)) {}
        
    };

    template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey,class Alloc = awstl::allocator<_hashtable_node<Value>>>
    struct _hashtable_iterator
    {
        using iterator = _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using const_iterator = _hashtable_iterator<const Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using node = _hashtable_node<Value>;
        using node_alloc = Alloc;
        using hashtable = hashtable<Value, Key, HashFcn, EqualKey, EqualKey, Alloc>;
        using original_iterator = aw_iterotor<Value ,ptrdiff_t, forward_iterator_tag, Value*, Value&>;
        using reference = Value&;
        using pointer = Value*;
        using iterator_category =  forward_iterator_tag;
        using value_type = Value;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        
        node* cur;
        hashtable* ht;

        _hashtable_iterator(const node& n, hashtable* tab) : cur(&n), ht(tab) {}
        _hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
        reference operator*() const {return cur->value;}
        pointer operator->() const {return &(operator*());}

        iterator& operator++();
        iterator operator++(int);

        bool operator==(const iterator& it) const {return cur == it.cur;}
        bool operator!=(const iterator& it) const {return cur != it.cur;}

        
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    typename _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator& _hashtable_iterator<Value,
    Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++()
    {
        if(cur->next)
        {
            cur = cur->next;
            return *this;
        }
        size_type bucket = ht->bkt_num(ExtractKey()(cur->value));
        while(!cur && ++bucket < ht->buckets.size())
        {
            cur = ht->buckets[bucket];
        }
        return *this;
    }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    typename _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator _hashtable_iterator<Value,
    Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }
    

#define sethead(n,bt,k) n->next = bt[k]; \
                        bt[k] = n;
    
    template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey,class Alloc = awstl::allocator<_hashtable_node<Value>>>
    class hashtable
    {
        using node = _hashtable_node<Value>;
        using node_alloc = Alloc;
        using iterator = _hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using value_type = Value;
        using key_type = Key;
    public:
        using size_type = size_t;
        using hasher = HashFcn;
        using key_equal = EqualKey;
    private:
        hasher hash;
        key_equal equals;
        ExtractKey get_key;

        std::vector<node*> buckets;
        size_type num_elements;

#pragma region prime_list
        static const int __stl_num_primes = 28;
        static const unsigned long long __stl_prime_list[__stl_num_primes] = {
        53,         97,         193,        389,        769,
            1543,       3079,       6151,       12289,      24593,
            49157,      98317,      196613,     393241,     786433,
            1572869,    3145739,    6291469,    12582917,   25165843,
            50331653,   100663319,  201326611,  402653189,  805306457,
            1610612741, 3221225473ul, 4294967291ul
        };

        static inline unsigned long long __stl_next_prime(unsigned long long n)
        {
            const unsigned long long* first = __stl_prime_list;
            const unsigned long long* last = __stl_prime_list + __stl_num_primes;
            const unsigned long long* pos = std::lower_bound(first, last, n);
            return pos == last ? *(last - 1) : *pos;
        }
        
#pragma endregion

    protected:
        node* get_node(const value_type& v)
        {
            auto np = node_alloc::allocate();
            try
            {
                construct(&np->value, v);
                return np;
            }
            catch (...)
            {
                node_alloc::destroy(&np->value);
                node_alloc::deallocate(np);
                throw;
            }
        }
        void put_node(node* _node)
        {
            node_alloc::destroy(&_node->value);
            node_alloc::deallocate(_node);
        }
    public:
        size_type bucket_count() const {return buckets.size();}
        unsigned long long max_bucket_count() const {return __stl_prime_list[__stl_num_primes - 1];}
        size_type size() const {return num_elements;}

#pragma region ctor
        void initialize_buckets(size_type n)
        {
            auto n_buckets = __stl_next_prime(n);
            buckets = awstl::vector<node>(n_buckets);
            buckets.resize(n_buckets, nullptr);
        }
        
        hashtable(size_type n, const HashFcn& hf, const EqualKey& eql) : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
        {
            initialize_buckets(n);
        }
#pragma endregion

    protected:

        void resize(size_type new_size)
        {
            auto old_n = buckets.size();
            if(new_size < old_n) return;
            auto n = __stl_next_prime(new_size);
            auto temp = std::vector<node*>(n, nullptr);
            for(auto i = 0; i < old_n; ++i)
            {
                if(buckets[i] != nullptr)
                {
                    auto head = buckets[i];
                    while(head)
                    {
                        buckets[i] = head->next;
                        sethead(head,temp, bkt_num(head->value, n));
                        head = buckets[i];
                    }
                }
            }
            std::swap(temp, buckets);
        }

        size_type bkt_num(const key_type& key, size_type n) const
        {
            return hash(key) % n;
        }

        size_type bkt_num(const key_type& key) const
        {
            return bkt_num(key, buckets.size());
        }

        size_type bkt_num(const value_type& obj, size_type n) const
        {
            return bkt_num(get_key(obj), n);
        }
        
        size_type bkt_num(const value_type& obj) const
        {
            return bkt_num(get_key(obj), buckets.size());
        }
    
    public:

        std::pair<iterator, bool> insert_unique_noresize(const value_type& obj)
        {
            const auto n = bkt_num(obj);
            auto first = buckets[n];
            for(; first; first = first->next)
            {
                if(equals(get_key(first->value), get_key(obj)))
                {
                    return std::pair<iterator, bool>(iterator(first, this), false);
                }
            }
            auto temp = get_node(obj);
            sethead(temp, buckets, n);
            ++num_elements;
            return std::pair<iterator, bool>(iterator(temp, this), true);
        }
        
        std::pair<iterator, bool> insert_unique(const value_type& obj)
        {
            resize(num_elements + 1);
            return insert_unique_noresize(obj);
        };

        std::pair<iterator, bool> insert_equal_noresize(const value_type& obj)
        {
            const auto n = bkt_num(obj);
            auto curr = buckets[n];
            for(; curr; curr = curr->next)
            {
                if(equals(get_key(curr->value), get_key(obj)))
                {
                    node* tmp = get_node(obj);
                    tmp->next = curr->next;
                    curr->next = tmp;
                    ++num_elements;
                    return std::pair<iterator, bool>(iterator(tmp, this), true);
                }
            }
            auto temp = get_node(obj);
            sethead(temp, buckets, n);
            ++num_elements;
            return std::pair<iterator, bool>(iterator(temp, this), true);
        }
        
        std::pair<iterator, bool> insert_equal(const value_type& obj)
        {
            resize(num_elements + 1);
            return insert_equal_noresize(obj);
        };
    };
    
   
}

#endif

