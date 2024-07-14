#include <iostream>
#include <vector>
#include "include/allocator.hpp"
#include "include/uninitialized.hpp"

int main(int argc, char* argv[])
{
    int ia[5] = {0,1,2,3,4};
    int ib[6] = {5,6,7,8,9,10};
    unsigned int i = 0;
    
    std::vector<int,awstl::allocator<int> > iv(ia,ia+5);
    std::vector<int> ivv(ib,ib+2);
    for(i = 0; i < iv.size(); i++)
    {
        std::cout << iv[i] << " ";
    }
    std::cout << std::endl;

    awstl::uninitialized_copy(iv.begin(),iv.end(),ivv.begin(),ivv.end());
    for(i = 0; i < ivv.size(); i++)
    {
        std::cout << ivv[i] << " ";
    }
    
    return 0;
}

