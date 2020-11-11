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

    ~Cache(){}
};

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

//! house keeping stuff
vector<Cache> caches;
vector<vector<int>> lru_bits(16384 , vector<int>(4 , -1));

void lru_policy(string address){
    string tag=address.substr(0,16);
    string index = address.substr(16 , 14);
    string offset = address.substr(30 , 2);

    int index_location = binaryToDecimal(index);

    bool cache_updated = false;
    int max_lru_set_bit = -1;

    for(int i=0 ; i<4 ; ++i){
        if(lru_bits[index_location][i] == -1 || caches[i]._validate[index_location]==false){
            for(int j=0; j<4 ; ++j){
                // max_lru_set_bit = max(max_lru_set_bit , lru_bits[index_location][j]);
                if(max_lru_set_bit < lru_bits[index_location][j]){
                    max_lru_set_bit = lru_bits[index_location][j];
                }
            }
            lru_bits[index_location][i] = max_lru_set_bit+1;
            caches[i]._tag[index_location] = tag;
            miss();
            cache_updated = true;
            caches[i]._validate[index_location] = true;
            break;
        }

        if(caches[i]._tag[index_location]==tag and caches[i]._validate[index_location]==true){
            hit();
            for(int j=0 ; j<4; ++j){
                // max_lru_set_bit = max(max_lru_set_bit, lru_bits[index_location][j]);
                if(max_lru_set_bit < lru_bits[index_location][j]){
                    max_lru_set_bit = lru_bits[index_location][j];
                }
            }
            lru_bits[index_location][i] = max_lru_set_bit+1;
            caches[i]._validate[index_location]= true;
            cache_updated = true;
            break;
        }
        
        // max_lru_set_bit = max(max_lru_set_bit , lru_bits[index_location][i]);
        if(max_lru_set_bit < lru_bits[index_location][i]){
            max_lru_set_bit = lru_bits[index_location][i];
        }
    }

    if(cache_updated)
        return;

    miss();
    int lowest_lru_bit = numeric_limits<int>::max();
    int replace_target = -1;
    for(int i=0 ; i<4; ++i){
        if(lowest_lru_bit > lru_bits[index_location][i]){
            lowest_lru_bit = lru_bits[index_location][i];
            replace_target=i;
        }
    }
    lru_bits[index_location][replace_target] = max_lru_set_bit+1;
    caches[replace_target]._tag[index_location] = tag;
}

void execute(string fileName){

}

int main(){

    vector<Operation> operations;
    
    string fileName = "gcc.trace";
    string filePath = "../traces/" + fileName;

    std::cout << "opened the file for reading " << endl;
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

    file.close();   
    std::cout << "closed the file" << endl;

    int targetSize = (int)pow(2 , 14);

    Cache cache1(targetSize);
    Cache cache2(targetSize);
    Cache cache3(targetSize);
    Cache cache4(targetSize);

    caches.push_back(cache1);
    caches.push_back(cache2);
    caches.push_back(cache3);
    caches.push_back(cache4);

    std::cout << caches.size() << endl;

    for(int i=0; i<(int)operations.size() ; ++i){
        string address = operations[i]._address;
        string binAddress = hextobin(address);
        lru_policy(binAddress);
    }

    std::cout << "hit: " << hitCount <<endl;
    std::cout << "miss: " << missCount <<endl;

    return 0;
}