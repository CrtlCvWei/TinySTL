#pragma once
#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_
#include <assert.h>
#include <valarray>

#include "allocator.hpp"
#include "uninitialized.hpp"
#include "utils.hpp"

namespace awstl
{
    template <class T>
    class vector
    {
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in awstl");

    protected:
        using data_alloctor = awstl::allocator<T>;

        using value_type = T;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = awstl::reverse_iterator<iterator>;
        using const_reverse_iterator = awstl::reverse_iterator<const_iterator>;
        using reference = T&;
        using size_type = typename data_alloctor::size_type;
        using ptrdiff_t = typename data_alloctor::difference_type;


        iterator start;
        iterator finish;
        iterator end_of_storage;

        void insert_aux(iterator position, const T& x);

        void deallocate()
        {
            if (start)
            {
                data_alloctor::deallocate(start, end_of_storage - start);
            }
        }

        void init_constuct()
        {
            try
            {
                start = data_alloctor::allocate(16);
                finish = start;
                end_of_storage = start + 16;
            }
            catch (...)
            {
                start = finish = end_of_storage = nullptr;
                throw;
            }
        }

        iterator allocate_and_fill(size_type n, const T& x);

        iterator reallocate_and_copy(T* new_start, size_type new_size)
        {
            T* new_finish = new_start;
            try
            {
                new_finish = uninitialized_copy(start, start + new_size, new_start, new_start + new_size);
                return new_finish;
            }
            catch (...)
            {
                for (T* it = new_start; it != new_finish; ++it)
                {
                    data_alloctor::destroy(it);
                }
                data_alloctor::deallocate(new_start, new_size);
                throw;
            }
        }

        void fill_initialize(size_type n, const T& value);

    public:
#pragma region constrcutor
        vector()
        {
            init_constuct();
        }

        vector(size_type n, const T& value) { fill_initialize(n, value); }
        vector(int n, const T& value) { fill_initialize(n, value); }
        vector(long n, const T value) { fill_initialize(n, value); }
        explicit vector(size_type n) { fill_initialize(n, T()); }

        vector(iterator forward, iterator last)
        {
            const auto n = last - forward;
            const auto _size = awstl::round_up(n, 8);
            start = data_alloctor::allocate(_size);
            finish = awstl::uninitialized_copy(forward, last, start, start + _size);
            end_of_storage = start + _size;
        }

        vector(const vector& x) //copy constrcut
        {
            start = data_alloctor::allocate(x.size());
            finish = uninitialized_copy(x.begin(), x.end(), start);
            end_of_storage = finish;
        }

        vector(vector&& x) noexcept : start(std::move(x.start)), finish(std::move(x.finish)), end_of_storage(x.end_of_storage)
        {
            x.start = x.finish = x.end_of_storage = nullptr;
        }

        T& operator=(const vector& x)
        {
            if (this != &x)
            {
                if (x.size() > capacity())
                {
                    iterator new_start = data_alloctor::allocate(x.size());
                    iterator new_finish = uninitialized_copy(x.begin(), x.end(), new_start);
                    destroy(start, finish);
                    data_alloctor::deallocate(start, end_of_storage - start);
                    start = new_start;
                    finish = new_finish;
                    end_of_storage = start + x.size();
                }
                else if (size() >= x.size())
                {
                    iterator new_finish = std::copy(x.begin(), x.end(), start);
                    destroy(new_finish, finish);
                    finish = new_finish;
                }
                else
                {
                    std::copy(x.begin(), x.begin() + size(), start);
                    uninitialized_copy(x.begin() + size(), x.end(), finish);
                }
            }
            return *this;
        }
#pragma endregion

        ~vector()
        {
            destroy(start, finish);
            deallocate();
        }

        iterator begin() noexcept { return start; }
        iterator end() noexcept { return finish; }
        const_iterator begin() const noexcept { return const_cast<const_iterator>(start); }
        const_iterator end() const noexcept { return const_cast<const_iterator>(finish); }
        reverse_iterator rend() noexcept { return reverse_iterator(start); }
        reverse_iterator rbegin() noexcept { return reverse_iterator(finish); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(start); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(finish); }

        size_type size() const { return static_cast<size_type>(end() - begin()); }
        bool empty() const { return begin() == end(); }
        size_type capacity() const { return static_cast<size_type>(end_of_storage - start); }
        reference operator[](size_type n) { return *(begin() + n); }
        reference front() { return *begin(); }
        reference back() { return *(end() - 1); }

        iterator insert(iterator pos, const T& x)
        {
            assert(pos >= begin() && pos < end());
            if (finish != end_of_storage)
            {
                data_alloctor::construct(finish, *(finish - 1));
                std::copy_backward(pos, finish - 1, finish);
                *pos = x;
                ++finish;
            }
            else
            {
                insert_aux(pos, x);
            }
            return pos;
        }

        void insert(iterator pos, size_type n, const T& x)
        {
            if (n == 0)
                return;
            if (end_of_storage - finish >= n)
            {
                const T x_copy = x;
                const size_type elem_after = finish - pos;
                const auto old_finish = finish;
                if (elem_after >= n)
                {
                    uninitialized_copy(finish - n, finish, finish, finish + n);
                    uninitialized_copy(pos, finish - n, pos + n, finish);
                    uninitialized_fill(pos, pos + n, x_copy);
                }
                else
                {
                    uninitialized_copy(pos, finish, pos + n, finish + n);
                    uninitialized_fill_n(old_finish, n - elem_after, x_copy);
                    uninitialized_fill(pos, old_finish, x_copy);
                }
                finish += n;
            }
            else
            {
                auto x_copy = x;
                const size_type old_size = size();
                const size_type new_size = old_size + std::max(old_size, n);
                iterator new_start = data_alloctor::allocate(new_size);
                iterator new_finish = new_start;
                try
                {
                    new_finish = uninitialized_copy(start, pos, new_start, new_start + (pos - start));
                    uninitialized_fill_n(new_finish, n, x_copy);
                    new_finish = uninitialized_copy(pos, finish, new_finish, new_start + new_size);
                }
                catch (...)
                {
                    destroy(new_start, new_finish);
                    data_alloctor::deallocate(new_start, new_size);
                    throw;
                }
                destroy(start, finish);
                deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + new_size;
            }
        }

        void push_back(const T& value)
        {
            if (finish != end_of_storage)
            {
                data_alloctor::construct(finish, value);
                ++finish;
            }
            else
            {
                insert_aux(finish, value);
            }
        }

        void pop_back()
        {
            --finish;
            destroy(finish);
        }

        iterator erase(iterator postion)
        {
            if (postion + 1 != end())
            {
                std::copy(postion + 1, finish, postion);
            }
            destroy(--finish);
            return postion;
        }

        iterator erase(iterator first, iterator last)
        {
            iterator i = std::copy(last, finish, first);
            destroy(i, finish);
            finish = finish - (last - first);
            return first;
        }

        iterator clear() { return erase(begin(), end()); }

        void resize(size_type new_size, const T& x)
        {
            if (new_size < size())
            {
                erase(start + new_size, finish);
                finish = start + new_size;
            }
            else if (new_size > size())
            {
                insert(end(), new_size - size(), x);
            }
        }
    };

    template <class T>
    typename awstl::vector<T>::iterator
    vector<T>::allocate_and_fill(size_type n, const T& x)
    {
        iterator result = data_alloctor::allocate(n);
        uninitialized_fill_n(result, n, x);
        return result;
    }

    template <class T>
    void awstl::vector<T>::fill_initialize(size_type n, const T& value)
    {
        start = allocate_and_fill(n, value);
        end_of_storage = finish = start + n;
    }

    template <class T>
    void vector<T>::insert_aux(iterator position, const T& x)
    {
        // always call when finish -> end_of_storage
        if (finish != end_of_storage)
        {
            data_alloctor::construct(finish, static_cast<reference>(*(finish - 1)));
            std::copy_backward(position, finish - 1, finish);
            auto x_copy = x;
            *position = x_copy;
            ++finish;
        }
        else
        {
            const size_type old_size = size();
            const size_type new_size = old_size != 0 ? 2 * old_size : 1;

            auto new_start = data_alloctor::allocate(new_size);
            auto new_finish = reallocate_and_copy(new_start, static_cast<size_type>(position - start));
            if (!new_finish)
            {
                //error
                std::cerr << "vector::insert_aux  not enough memory" << std::endl;
            }
            else
            {
                auto x_copy = x;
                *new_finish = x_copy;
                ++new_finish;
                new_finish = uninitialized_copy(position, finish, new_finish, new_start + new_size);
                data_alloctor::deallocate(start, old_size);
                this->start = new_start;
                this->finish = new_finish;
                this->end_of_storage = new_start + new_size;
            }
        }
    }
}


#endif // !_VECTOR_HPP_
