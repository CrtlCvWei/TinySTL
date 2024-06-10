#include <iostream>
#include <vector>
#include "include/allocator.hpp"
#include "include/constructor.hpp"

int main(int argc, char* argv[])
{
    int ia[5] = {0,1,2,3,4};
    unsigned int i = 0;
    
    std::vector<int,awstl::allocator<int> > iv(ia,ia+5);
    for(i = 0; i < iv.size(); i++)
    {
        std::cout << iv[i] << " ";
    }
    return 0;
}

