#include<bits/stdc++.h>
using namespace std;

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

struct Operation{
    //! we gonna ignore the last field for now => issues in parsing, also it was not relevant to any calculations 
    char _type;
    std::string _address;
    Operation(char type , std::string address){
        _type = type;
        _address = address;
    }
};

string hextobin(const string &s){
    string out;
    for(auto i: s){
        uint8_t n;
        if(i <= '9' and i >= '0')
            n = i - '0';
        else
            n = 10 + i - 'A';
        for(int8_t j = 3; j >= 0; --j)
            out.push_back((n & (1<<j))? '1':'0');
    }
    return out.substr(8,32);
}

int binaryToDecimal(string n){
    string num = n;
    int dec_value = 0;
 
    // Initializing base value to 1, i.e 2^0
    int base = 1;
 
    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }
    return dec_value;
}

int hitCount=0;
int missCount=0;

void hit(){
    ++hitCount;
}

void miss(){
    ++missCount;
}

int main(){

    vector<Operation> operations;
    
    string fileName = "gcc.trace";
    string filePath = "../traces/" + fileName;

    std::ifstream file(filePath);
    std::string current;
    bool parser=true;
    
    while(std::getline(file , current)){
        char type = current[0];
        std::string address = current.substr(2 ,10);
        
        Operation current_op(type,address);
        operations.push_back(current_op);

        if(type != current[0] || address != current.substr(2 , 10) || current_op._type != type || current_op._address != address){
            parser = false; 
            break;
        }
    }

    

    return 0;
}