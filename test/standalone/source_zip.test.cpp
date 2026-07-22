// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <limits>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/utility/source_zip.hpp"

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

void test_varuint_and_integer_api(){
    using source_zip_internal::append_varuint;
    using source_zip_internal::base64_value;
    using source_zip_internal::decode_integer;
    using source_zip_internal::encode_integer;
    using source_zip_internal::read_varuint;

    assert(base64_value('A') == 0);
    assert(base64_value('Z') == 25);
    assert(base64_value('a') == 26);
    assert(base64_value('9') == 61);
    assert(base64_value('-') == 62);
    assert(base64_value('_') == 63);
    assert(base64_value('!') == -1);

    const std::vector<std::uint64_t> values = {
        0,
        1,
        127,
        128,
        16384,
        std::numeric_limits<std::uint64_t>::max(),
    };
    std::vector<unsigned char> bytes;
    for(std::uint64_t value: values) append_varuint(bytes, value);
    std::size_t position = 0;
    for(std::uint64_t value: values){
        assert(read_varuint<std::uint64_t>(bytes, position) == value);
    }
    assert(position == bytes.size());

    for(std::int64_t value: {
        std::numeric_limits<std::int64_t>::min(),
        std::int64_t{-1},
        std::int64_t{0},
        std::int64_t{1},
        std::numeric_limits<std::int64_t>::max(),
    }){
        assert(decode_integer<std::int64_t>(encode_integer(value)) == value);
    }
    assert(decode_integer<std::uint64_t>(encode_integer(
        std::numeric_limits<std::uint64_t>::max()
    )) == std::numeric_limits<std::uint64_t>::max());

    expect_source_zip_error([]{
        const std::vector<unsigned char> truncated{0x80};
        std::size_t position = 0;
        (void)read_varuint<std::uint64_t>(truncated, position);
    });
    expect_source_zip_error([]{
        const std::vector<unsigned char> overflow{0x80, 0x02};
        std::size_t position = 0;
        (void)read_varuint<std::uint8_t>(overflow, position);
    });
}

void test_malformed_source_zip(){
    using source_zip_internal::encode_base64;
    expect_source_zip_error([&]{
        (void)source_unzip<std::uint8_t>(encode_base64({1}));
    });
    expect_source_zip_error([&]{
        (void)source_unzip<std::uint8_t>(encode_base64({1, 0, 0}));
    });
    expect_source_zip_error([&]{
        (void)source_unzip<std::uint8_t>(encode_base64({0, 0}));
    });
    expect_source_zip_error([&]{
        (void)source_unzip<std::uint8_t>(encode_base64({1, 1, 0x80, 0x02}));
    });
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
    test_varuint_and_integer_api();
    test_malformed_source_zip();
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
