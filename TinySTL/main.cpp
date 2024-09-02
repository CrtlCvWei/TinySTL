#include <iostream>
#include <vector>
#include "include/allocator.hpp"
#include "include/list.hpp"
#include "include/uninitialized.hpp"
#include "include/deque.hpp"
#include "include/stack.hpp"

int main(int argc, char* argv[])
{

    // awstl::deque<int,awstl::allocator<int>, 32> ideq(20, 9);
    //
    // ideq.push_back(10);
    // ideq.push_front(100);
    //
    // std::cout << "ideq.size() = " << ideq.size() << std::endl;
    //
    // for( auto it = ideq.begin(); it != ideq.end(); ++it)
    // {
    //     std::cout << *it << " ";
    // }
    //
    // ideq.pop_back();
    // std::cout << std::endl;
    // std::cout << "ideq.size() = " << ideq.size() << std::endl;
    // for( auto it = ideq.begin(); it != ideq.end(); it++)
    // {
    //     std::cout << *it << " ";
    // }
    //
    // ideq.pop_back();
    // std::cout << std::endl;
    // std::cout << "ideq.size() = " << ideq.size() << std::endl;
    // for(auto it = ideq.begin(); it != ideq.end(); it++)
    // {
    //     std::cout << *it << " ";
    // }
    //
    // ideq.clear();
    // std::cout << std::endl;
    // std::cout << "ideq.size() = " << ideq.size() << std::endl;
    // for( auto it = ideq.begin(); it != ideq.end(); ++it)
    // {
    //     std::cout << *it << " ";
    // }
    //
    return 0;
}

