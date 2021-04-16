#include<bits/stdc++.h>
#include "imt2019062_MathUtilities.h"
#include "imt2019062_Cache.h"
#include "imt2019062_Operation.h"

/*
    compiling instructions 
        $ g++ imt2019062_main.cpp
        $ ./a.out
*/

//! global counters to store the hit and miss
int hitCount=0;
int missCount=0;
//! declaring a variable for counting the #miss ( miss= total-hit ) is not exactly smart, but has been just for the code to look complete

//! increment on hit
void hit(){
    ++hitCount;
}

//! increment on miss
void miss(){
    ++missCount;
}

void execute(std::string fileName){

    //! start from a count of zero in every iteration of execute() as execute is called on every trace file
    hitCount=0;
    missCount=0;

    //! parse the entire input file into a vector of structures containing only the relevant information => Operation type, address 
    /*  
        refer to Operation.h for the complete implementation

        struct Operation{
            char type;
            string _address;
        };
    */
    std::vector<Operation> operations;

    //! specify the input file to parse => it is important that the file path is not distrubed, else an error in program execution
    std::string filePath = "../traces/" + fileName;

    //! open the file and read an entire line into a string 
    std::ifstream file(filePath);
    std::string current;
    bool parser=true;
    
    while(std::getline(file , current)){
        //! read the file line by line and store each line in a string, retrieve the address after parsing 
        char type = current[0];
        std::string address = current.substr(2 ,10);
        
        //! make a unit of the line and store it a an instruction to execute later 
        Operation current_op(type,address);
        operations.push_back(current_op);

        //! check if the integrity of the input, if not stored correctly, means the input file is not to the exact specifications, 
        //! break here and execute for the next files.
        if(type != current[0] || address != current.substr(2 , 10) || current_op._type != type || current_op._address != address){
            parser = false; 
            break;
        }
    }

    //! the cache object is a sort of "pseudo" cache that allows us to access the validate fields and perform tag matches
    //! it is not the implementation of a real cache as it does not hold blocks of data
    int cache_size = (int)std::pow(2, 16);
    Cache cache(cache_size);
    /*  
        refer to Cache.h for the complete implementation
        
        struct Cache{
            vector<bool> _validate;
            vector<string> _address;
        };
    */

    for(int i=0 ; i<(int)operations.size() ; ++i){
        //! get the address (Hex) from out instruction set parsed earlier from the entire file.
        std::string address = operations[i]._address;
        //! convert to binary
        std::string address_block = hextobin(address);
        
        //! tag=14 , index=16 , offset=2 => get all the relevant information from the binary equivalent
        std::string tag = address_block.substr(0,14);
        std::string index = address_block.substr(14,16);
        std::string byte_offset = address_block.substr(30,2);

        //! check tag integrity => if compromised => means the file is not valid => return and move onto the next file to execute
        if(tag.length() != 14){
            std::cout << "Invalid Input File" << std::endl;
            return;
        }
        //! check index integrity => if compromised => means the file is not valid => return and move onto the next file to execute
        if(index.length() != 16){
            std::cout << "Invalid Input File" << std::endl;
            return;
        }

        //! get the index location in the cache 
        int index_location = binaryToDecimal(index);
        
        //! if initially the index location is invalid => miss();
        if(cache._validate[index_location] == false){
            miss();
            cache._validate[index_location] = true;
            cache._tag[index_location] = tag;
        }
        else{
            //! valid location set valid to true (it was true earlier also as we are in the else block), and perform the tag match => equal => hit() 
            cache._validate[index_location] = true;
            if(cache._tag[index_location] == tag){
                hit();
            }
            else{
                //! valid location but the tag did not match => miss()
                miss();
                cache._validate[index_location] = true;
                cache._tag[index_location] = tag;
            }
        }
    }

    //!check if the integrity of the input, if not stored correctly, means the input file is not to the exact specifications, 
    //! this prompt is given in case of an invalid file 
    if(parser){
        std::cout<<"Done!"<<std::endl;
    }
    else{
        std::cout<<"Check if the trace file was modified or not? any extra space or line can cause the program to crash"<<std::endl; 
        return;
    }


    //! report hit and miss counts 
    std::cout << "HitCount: " << hitCount << std::endl;
    std::cout << "MissCount: " << missCount << std::endl;

    //! compute corresponding rates 
    long double HitRate = (float)hitCount/(hitCount+missCount);
    long double MissRate = (float)missCount/(hitCount+missCount);

    //! report hit rate and miss rate as a fraction
    std::cout << "HitRate: " << HitRate << std::endl;
    std::cout << "MissRate: " << MissRate << std::endl;
}

int main(){

    //! the prompts are explanatory, we execute the required files in order 
    //! the execute(fileName) function is the core of the implementation and performs all the required computation

    std::cout << "executing for gcc.trace" << std::endl;
    execute("gcc.trace");
    std::cout << "successfully executed" << std::endl;
    std::cout << "<----->" << std::endl;

    std::cout << "executing for gzip.trace" << std::endl;
    execute("gzip.trace");
    std::cout << "successfully executed" << std::endl;
    std::cout << "<----->" << std::endl;

    std::cout << "executing for mcf.trace" << std::endl;
    execute("mcf.trace");
    std::cout << "successfully executed" << std::endl;
    std::cout << "<----->" << std::endl;

    std::cout << "executing for swim.trace" << std::endl;
    execute("swim.trace");
    std::cout << "successfully executed" << std::endl;
    std::cout << "<----->" << std::endl;

    std::cout << "executing for twolf.trace" << std::endl;
    execute("twolf.trace");
    std::cout << "successfully executed" << std::endl;

    return 0;
}
