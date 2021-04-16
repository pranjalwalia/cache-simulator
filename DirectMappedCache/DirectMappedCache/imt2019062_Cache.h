#ifndef CACHE_H
#define CACHE_H
//! define guards

#include<bits/stdc++.h>

struct Cache{
    std::vector<bool> _validate; 
    std::vector<std::string> _tag;
    Cache(int size){
        _validate.resize(size);
        _tag.resize(size);
        for(int i=0 ; i<size; ++i){
            _validate[i] = false;
        }
    }
};

#endif