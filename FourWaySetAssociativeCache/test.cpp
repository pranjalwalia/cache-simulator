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
    int base = 1;
    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }
    return dec_value;
}

int hit=0;
int miss=0;

//!house keeping 
vector<vector<string>> cache(16384 , vector<string>(4 , "-1"));
vector<vector<int>> lru_bit(16384 , vector<int>(4 , -1));

void lru_scheme(string address){
    string tag=address.substr(0,16);
    string index = address.substr(16 , 14);
    string offset = address.substr(30 , 2);

    int index_location = binaryToDecimal(index);

    int isput = -1;
    int maxi = -1;

    for(int i=0 ; i<4 ; ++i){
        if(lru_bit[index_location][i] == -1){
            for(int j=0 ; j<4 ;++j){
                maxi = max(maxi , lru_bit[index_location][j]);
            }
            lru_bit[index_location][i] = maxi+1;
            cache[index_location][i] = tag;
            ++miss;
            isput=1;
            break;
        }

        if(cache[index_location][i] == tag){
            ++hit;
            for(int j=0 ; j<4 ; ++j){
                maxi = max(maxi , lru_bit[index_location][j]);
            }
            lru_bit[index_location][i] = maxi+1;
            cache[index_location][i] = tag;
            isput=1;
            break;
        }

        maxi = max(maxi , lru_bit[index_location][i]);
    }    

    if(isput == 1){
        return;
    }

    ++miss;
    int mini = 1e9;
    int toreplace=-1;
    for(int i=0 ; i<4; ++i){
        if(mini > lru_bit[index_location][i]){
            mini = lru_bit[index_location][i];
            toreplace=i;
        }
    }
    lru_bit[index_location][toreplace] = maxi+1;
    cache[index_location][toreplace] = tag;
}

int main(){
        //! 14(index) , offset(2) , tag(16), 4way
    vector<Operation> operations;
    string fileName = "gcc.trace";
    string filePath = "../traces/" + fileName;
    std::ifstream file(filePath);
    std::string current;
    cout << "opened the file" << endl;
    while(std::getline(file , current)){
        char type = current[0];
        std::string address = current.substr(2 ,10);
        Operation current_op(type,address);
        operations.push_back(current_op);
    }
    file.close();
    cout << "closed the file, operations size: " << operations.size() << endl;

    for(int i=0 ; i<(int)operations.size() ; ++i){
        string address = operations[i]._address;
        string binAddress=hextobin(address);
        lru_scheme(binAddress);
    }

    cout << "hit: " << hit << endl;
    cout << "miss: " << miss << endl;
    return 0;
}
