#include<bits/stdc++.h>
#include "Cache.h"
#include "Operation.h"
#include "MathUtilities.h"

//! define global count, remmeber to reset at every new trace file
int hitCount=0;
int missCount=0;

//! increment hits in a hit event
void hit(){
    ++hitCount;
}

void miss(){
    ++missCount;
}

//! declare global characteristice
int target_number_sets = (int)std::pow(2 , 14);

//! init global vector to hold the four caches, all computation is done on this 
std::vector<Cache> caches;

//! init the LRU bits required to implement the LRU replacement policy
std::vector<std::vector<int>> lru_bits(target_number_sets , std::vector<int>(4 , -1));


/*

                                                <== 4 ways ==>
        -----------------------------------------------------------------------------------------------------
        | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> |   => LRU { -1 , -1 , -1 , -1 }
        | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> |   => LRU { -1 , -1 , -1 , -1 }  
        | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> |   => LRU { -1 , -1 , -1 , -1 }
        | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> |   => LRU { -1 , -1 , -1 , -1 }
        | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> | <valid>  <tag> |   => LRU { -1 , -1 , -1 , -1 }
        ...
        ...
        ...
        ...
        upto 2^14 rows in this case 
        -----------------------------------------------------------------------------------------------------
*/


/*
    For a given set, In the given implementation LRU bits do not serve as actual bits i.e. (0/1).
    Instead, they keep track of the count since a given way was last used
    Therefore, least lru bit => least recently used and highest => most recently used


    LRU_BITS = {-1 , -1 , -1 , -1} => initial condition, -1 indicates a false <valid>   <fie>ld
    
    LRU_BITS = {1 , 0 , 0 , 0} => first way was used most recently

    LRU_BITS = {1 , 0 , 2 , 1 , 1} => the third way is most recently used and second is is least (to be replaced!)

*/

void lru_policy(std::string address){

    //! get required index and tag in binary
    std::string tag=address.substr(0,16);
    std::string index = address.substr(16 , 14);
    std::string offset = address.substr(30 , 2);

    //! get index in decimal form to locate the desired set
    int index_location = binaryToDecimal(index);

    //! boolean to check if our cache was updated or not, i.e fetch value from memory and put into the cache
    bool cache_updated = false;

    //! keeps track of the maximum value of lru bit i.e. the "way" corresponding to the highest bit gets updated
    int max_lru_set_bit = -1;

    for(int i=0 ; i<4 ; ++i){
        if(!caches[i]._validate[index_location] || lru_bits[index_location][i] == -1 ){
            //! Miss as validate == -1

            for(int j=0; j<4 ; ++j){
                //! get the max value of lru bit among the four
                if(max_lru_set_bit < lru_bits[index_location][j]){
                    max_lru_set_bit = lru_bits[index_location][j];
                }
            }            

            //! set the bit of the updated "way" as max because the updated way has the bit value among the 4 bits as highest(updated recently)
            lru_bits[index_location][i] = max_lru_set_bit+1;
            caches[i]._tag[index_location] = tag;
            miss();
            cache_updated = true;
            caches[i]._validate[index_location] = true;
            break;
        }

        if(caches[i]._tag[index_location]==tag and caches[i]._validate[index_location]){
            //! cache hit, i.e tag match and set valid field
            hit();
            for(int j=0 ; j<4; ++j){
                if(max_lru_set_bit < lru_bits[index_location][j]){
                    max_lru_set_bit = lru_bits[index_location][j];
                }
            }

            //! in case of cache hit, set the bit of target "way" as greater than max bit because it was used just now
            lru_bits[index_location][i] = max_lru_set_bit+1;
            // caches[i]._tag[index_location] = tag;
            caches[i]._validate[index_location]= true;
            cache_updated = true;
            break;
        }

        //! fetch the max bit => max bit indicates the most recently used.
        if(max_lru_set_bit < lru_bits[index_location][i]){
            max_lru_set_bit = lru_bits[index_location][i];
        }
    }

    //! if the cache was updated in this cycle, it means:
        //! 1. cache miss when cache was NOT full 
        //! 2. cache hit

    //! in case of both the above events we do not need to replace anything
    if(cache_updated)
        return;



    //! in the event of all ways full and miss => replacement required..
    //! Now fetch the lowest bit, in the event of getting all four ways full,
    //! the way corresponding to the least bit value if replaced
    miss();
    int lowest_lru_bit = numeric_limits<int>::max();
    int replace_target = -1;
    for(int i=0 ; i<4; ++i){
        if(lowest_lru_bit > lru_bits[index_location][i]){
            lowest_lru_bit = lru_bits[index_location][i];
            replace_target=i;
        }
    }

    //! after replacement, set the bit value to greater than the maximum as it was most recently used,
    lru_bits[index_location][replace_target] = max_lru_set_bit+1;
    caches[replace_target]._tag[index_location] = tag;
}


//! reset all the caches available and all the LRU bits, this is called for every new file
void reset(){
    hitCount=0;
    missCount=0;
    caches.clear();
    for(int i=0 ; i<target_number_sets ; ++i){
        for(int j=0 ; j<4 ; ++j){
            lru_bits[i][j]=-1;
        }
    }
}

void execute(string fileName){

    //! reset the cache and LRU bits
    reset();

    //! stores all the operations i.e. the addresses from the .trace files
    std::vector<Operation> operations;
    
    std::string filePath = "../traces/" + fileName;

    //! open desired file
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
    std::cout << "Done!" << std::endl;  //! done parsing the input


    //! instantiate 4 caches(a simple cache i.e one that constitutes a way) that form the 4 ways of the set associative cache
    Cache cache1(target_number_sets);
    Cache cache2(target_number_sets);
    Cache cache3(target_number_sets);
    Cache cache4(target_number_sets);

    //! push the caches( a simpler cache i.e one that constitutes a way in the entire 4 way set associative cache ) into a vector of all available caches 
    caches.push_back(cache1);
    caches.push_back(cache2);
    caches.push_back(cache3);
    caches.push_back(cache4);

    std::cout << "ways: " << caches.size() << std::endl;

    //! read all the input addresses and implement the LRU policy at every iteration 
    for(int i=0; i<(int)operations.size() ; ++i){
        std::string address = operations[i]._address;
        std::string binAddress = hextobin(address);
        lru_policy(binAddress);
    }

    //! print the outputs
    std::cout << "HitCount: " << hitCount <<std::endl;
    std::cout << "MissCount: " << missCount <<std::endl;

    std::cout << "HitRate: " << (double)hitCount/(hitCount+missCount) << std::endl;
    std::cout << "MissRate: " << (double)missCount/(hitCount+missCount) << std::endl;
}


int32_t main(){

    //! the prompts are explanatory, we execute the required files in order 
    //! the execute(fileName) function is the core of the implementation and performs all the required computation

    std::cout << "executing gcc.trace ..." << std::endl;
    execute("gcc.trace");
    std::cout << "successfully executed !" << std::endl;
    std::cout << "<------->" << std::endl;

    std::cout << "executing gzip.trace ..." << std::endl;
    execute("gzip.trace");
    std::cout << "successfully executed !" << std::endl;
    std::cout << "<------->" << std::endl;

    std::cout << "executing mcf.trace ..." << std::endl;
    execute("mcf.trace");
    std::cout << "successfully executed !" << std::endl;
    std::cout << "<------->" << std::endl;

    std::cout << "executing swim.trace ..." << std::endl;
    execute("swim.trace");
    std::cout << "successfully executed !" << std::endl;
    std::cout << "<------->" << std::endl;

    std::cout << "executing twolf.trace ..." << std::endl;
    execute("twolf.trace");
    std::cout << "successfully executed !" << std::endl;

    return 0;
}