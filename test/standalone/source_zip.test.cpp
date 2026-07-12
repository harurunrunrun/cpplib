// competitive-verifier: STANDALONE

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/other/source_zip.hpp"

template<class T>
void encode_values(int n){
    std::vector<T> values(static_cast<std::size_t>(n));
    for(T& value: values){
        long long input;
        std::cin >> input;
        value = static_cast<T>(input);
    }
    const std::string encoded = source_zip(values);
    std::cout << encoded << '\n';
    const auto restored = source_unzip<T>(encoded);
    std::cout << (restored == values) << '\n';
}

template<class T>
void decode_values(){
    std::string encoded;
    std::cin >> encoded;
    const auto values = source_unzip<T>(encoded);
    std::cout << values.size();
    for(T value: values) std::cout << ' ' << static_cast<long long>(value);
    std::cout << '\n';
}

int main(){
    std::string operation;
    if(!(std::cin >> operation)) return 0;
    if(operation == "ENCODE_I64"){
        int n;
        std::cin >> n;
        encode_values<std::int64_t>(n);
    }else if(operation == "ENCODE_U32"){
        int n;
        std::cin >> n;
        encode_values<std::uint32_t>(n);
    }else if(operation == "DECODE_I64"){
        decode_values<std::int64_t>();
    }else{
        decode_values<std::uint32_t>();
    }
}
