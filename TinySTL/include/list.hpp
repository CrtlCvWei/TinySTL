#pragma once
#ifndef _LIST_HPP_
#define _LIST_HPP_

// 这个头文件包含了一个模板类 list
// list : 双向链表

namespace awstl
{
    template <class T>
    struct _list_node
    {
        using value_type = T;
        list_node* prev;
        list_node* next;
        value_type data;
        list_node():prev(nullptr),next(nullptr),data(T()){}
    };
    
    
}


#endif
