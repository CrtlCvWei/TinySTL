#include <iostream>
#include <vector>
#include "include/allocator.hpp"
#include "include/uninitialized.hpp"
#include "include/vector.hpp"

int main(int argc, char* argv[])
{
    int ia[5] = {0,1,2,3,4};
    int ib[6] = {5,6,7,8,9,10};
    unsigned int i = 0;
    
    awstl::vector<int> iv(ia,ia+5);
    awstl::vector<int> ivv(ib,ib+2);
    for(i = 0; i < iv.size(); i++)
    {
        std::cout << iv[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "iv size: " << iv.size() << std::endl;

    for(int i = 5; i < 20; i++)
    {
        iv.push_back(i);
    }

    for(i = 0; i < iv.size(); i++)
    {
        std::cout << iv[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "iv size: " << iv.size() << std::endl;

    iv.insert(iv.begin()+2, 100);
    iv.resize(10,1);
    for(i = 0; i < iv.size(); i++)
    {
        std::cout << iv[i] << " ";
    }
    
    
    return 0;
}

