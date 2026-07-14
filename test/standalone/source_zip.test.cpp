// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/source_zip.hpp"

template<class Function>
void expect_source_zip_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_base64_api(){
    using source_zip_internal::decode_base64;
    using source_zip_internal::encode_base64;

    const std::vector<std::pair<std::vector<unsigned char>, std::string>> known = {
        {{}, ""},
        {{0x00}, "AA"},
        {{0xff}, "_w"},
        {{0x00, 0x01}, "AAE"},
        {{0xfb, 0xef, 0xff}, "--__"},
    };
    for(const auto& [bytes, encoded]: known){
        assert(encode_base64(bytes) == encoded);
        assert(decode_base64(encoded) == bytes);
    }

    std::mt19937_64 rng(0x50A2CEULL);
    for(std::size_t size = 0; size <= 257; ++size){
        std::vector<unsigned char> bytes(size);
        for(auto& byte: bytes) byte = static_cast<unsigned char>(rng());
        const std::string encoded = encode_base64(bytes);
        assert(encoded.find_first_not_of(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
        ) == std::string::npos);
        assert(decode_base64(encoded) == bytes);
    }

    expect_source_zip_error([]{ (void)decode_base64("A"); });
    expect_source_zip_error([]{ (void)decode_base64("!"); });
    expect_source_zip_error([]{ (void)decode_base64("AB"); });
}

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
    test_base64_api();
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
