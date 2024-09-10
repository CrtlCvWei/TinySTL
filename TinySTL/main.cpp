#include <iostream>
#include <vector>
#include "include/allocator.hpp"
#include "include/list.hpp"
#include "include/uninitialized.hpp"
#include "include/deque.hpp"
#include "include/queue.hpp"
#include "include/stack.hpp"
#include "include/rb_tree.hpp"
#include "include/set.hpp"


class Base
{
public:
    virtual void Fun()
    {
        std::cout << "Base" << std::endl;
    }
};

class Drived : public Base
{
public:
    virtual void Fun() override
    {
        std::cout << "Drived" << std::endl;
    }
};

using test_rb_tree = awstl::rb_tree<float, float, awstl::identity<float>, std::less<float>>;

int main(int argc, char* argv[])
{
    
   int i;
    int ia[5] = {0, 1, 2, 3, 4};
    awstl::set<int> iset(ia, ia+5);

    std::cout << "size = " << iset.size() << std::endl;
    iset.insert(3);
    std::cout << "size = " << iset.size() << std::endl;
    iset.insert(5);
    std::cout << "size = " << iset.size() << std::endl;
    iset.erase(1);
    std::cout << "size = " << iset.size() << std::endl;
    auto b1 = iset.begin();
    auto e1 = iset.end();
    while(b1 != e1)
    {
        std::cout << *b1 << " ";
        ++b1;
    }
    std::cout << std::endl;
    
}

