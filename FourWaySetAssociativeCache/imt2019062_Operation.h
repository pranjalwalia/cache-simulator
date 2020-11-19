#ifndef OPERATION_H
#define OPERATION_H
//! define guards

#include<bits/stdc++.h>

//! structure of a single line read from the trace file, we gonna use the _address attribute for computations

struct Operation{
    //! we gonna ignore the last field for now => issues in parsing, also it was not relevant to any calculations 
    char _type;
    std::string _address;
    Operation(char type , std::string address){
        _type = type;
        _address = address;
    }

    ~Operation(){}
};

#endif