#pragma once
#ifndef _ALGOBASE_HPP_
#define _ALGOBASE_HPP_

namespace awstl
{
    template<class T>
    const T& max(const T& a, const T& b)
    {
        return a < b ? b : a;
    }
    
}


#endif
