#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;


int main(){
    json j;
    j["p"] = 3.14;

    std::cout<<j.dump(4)<<std::endl;
    
    return 0;
}