// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/exact_integer.hpp"

namespace{

ExactInteger parse_hex(const std::string& token){
    if(token.empty()) throw std::invalid_argument("empty hexadecimal integer");
    std::size_t position = 0;
    bool negative = false;
    if(token[position] == '-' || token[position] == '+'){
        negative = token[position] == '-';
        ++position;
    }
    if(position == token.size()){
        throw std::invalid_argument("missing hexadecimal digits");
    }
    ExactInteger result = 0;
    for(; position < token.size(); ++position){
        const char digit = token[position];
        unsigned value = 0;
        if('0' <= digit && digit <= '9'){
            value = static_cast<unsigned>(digit - '0');
        }else if('a' <= digit && digit <= 'f'){
            value = static_cast<unsigned>(digit - 'a' + 10);
        }else if('A' <= digit && digit <= 'F'){
            value = static_cast<unsigned>(digit - 'A' + 10);
        }else{
            throw std::invalid_argument("invalid hexadecimal digit");
        }
        result <<= 4;
        result += value;
    }
    return negative ? -result : result;
}

void print_product_fingerprint(const ExactInteger& product){
    constexpr std::array<std::uint64_t, 3> divisors{
        1'000'000'007,
        4'294'967'291,
        (std::numeric_limits<std::uint64_t>::max)()
    };
    std::cout << product.bit_length() << ' '
              << static_cast<int>(product.is_negative());
    for(const std::uint64_t divisor: divisors){
        const auto [quotient, remainder] = product.divmod(divisor);
        const ExactInteger reconstructed =
            abs(quotient) * ExactInteger(divisor) + remainder;
        if(reconstructed != abs(product)){
            throw std::runtime_error(
                "ExactInteger divmod roundtrip failed"
            );
        }
        std::cout << ' ' << remainder;
    }
    std::cout << '\n';
}

template<class Integer>
void print_conversion(const ExactInteger& value){
    try{
        const Integer converted = value.checked_to<Integer>();
        std::cout << "OK " << ExactInteger(converted) << '\n';
    }catch(const std::overflow_error&){
        std::cout << "OVERFLOW\n";
    }
}

void self_test(){
    assert(ExactInteger().is_zero());
    assert(!ExactInteger().is_negative());
    assert(ExactInteger(0).bit_length() == 0);
    assert(ExactInteger(1).bit_length() == 1);
    assert(ExactInteger(-1).bit_length() == 1);
    assert(ExactInteger(-7).absolute() == 7);
    assert(abs(ExactInteger(-9)) == 9);
    assert(-ExactInteger(0) == 0);
    assert((ExactInteger(1) << 100).bit_length() == 101);
    assert((ExactInteger(-3) >> 1) == -2);
    assert((ExactInteger(-4) >> 1) == -2);
    assert((ExactInteger(-1) >> 1000) == -1);
    assert((ExactInteger(1) >> 1000) == 0);
    assert(ExactInteger(0).to_string() == "0");
    assert(ExactInteger(-1).to_string() == "-1");
    assert(((ExactInteger(1) << 32) - 1).to_string() == "4294967295");
    assert((ExactInteger(1) << 32).to_string() == "4294967296");
    assert(((ExactInteger(1) << 64) - 1).to_string()
        == "18446744073709551615");

    ExactInteger small = -5;
    small += std::uint64_t{9};
    assert(small == 4);
    small *= -3;
    small -= 2;
    assert(small == -14);

    const auto [negative_quotient, negative_remainder] =
        ExactInteger(-17).divmod(5);
    assert(negative_quotient == -3 && negative_remainder == 2);
    const auto [large_divisor_quotient, large_divisor_remainder] =
        (ExactInteger(1) << 127).divmod(
            (std::numeric_limits<std::uint64_t>::max)()
        );
    assert(large_divisor_quotient == (ExactInteger(1) << 63));
    assert(large_divisor_remainder == (std::uint64_t{1} << 63));

    assert(ExactInteger(-128).checked_to<std::int8_t>() == -128);
    assert(ExactInteger(255).checked_to<std::uint8_t>() == 255);
    assert(!ExactInteger(0).checked_to<bool>());
    assert(ExactInteger(1).checked_to<bool>());
    bool conversion_thrown = false;
    try{
        static_cast<void>(ExactInteger(256).checked_to<std::uint8_t>());
    }catch(const std::overflow_error&){
        conversion_thrown = true;
    }
    assert(conversion_thrown);

    bool zero_division_thrown = false;
    try{
        static_cast<void>(ExactInteger(1).divmod(0));
    }catch(const std::domain_error&){
        zero_division_thrown = true;
    }
    assert(zero_division_thrown);

#if defined(__SIZEOF_INT128__)
    const __uint128_t unsigned_maximum = ~__uint128_t{0};
    const __int128_t signed_minimum =
        -static_cast<__int128_t>(unsigned_maximum >> 1) - 1;
    assert(ExactInteger(unsigned_maximum).checked_to<__uint128_t>()
        == unsigned_maximum);
    assert(ExactInteger(signed_minimum).checked_to<__int128_t>()
        == signed_minimum);
#endif

    std::ostringstream stream;
    stream << ((ExactInteger(1) << 130) + 12345);
    assert(stream.str() == "1361129467683753853853498429727072858169");
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_test();

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "DEC"){
            std::string token;
            std::cin >> token;
            std::cout << parse_hex(token).to_string() << '\n';
        }else if(operation == "DEC_POWER"){
            std::size_t bits;
            std::int64_t offset;
            int negative;
            std::cin >> bits >> offset >> negative;
            if(negative != 0 && negative != 1) return 2;
            ExactInteger value = ExactInteger(1) << bits;
            value += offset;
            if(negative != 0) value = -value;
            std::cout << value.to_string() << '\n';
        }else if(operation == "META"){
            std::string token;
            std::cin >> token;
            const ExactInteger value = parse_hex(token);
            std::cout << value << ' ' << value.bit_length() << ' '
                      << value.is_negative() << ' ' << value.absolute() << '\n';
        }else if(operation == "NEG" || operation == "ABS"){
            std::string token;
            std::cin >> token;
            const ExactInteger value = parse_hex(token);
            std::cout << (operation == "NEG" ? -value : value.absolute()) << '\n';
        }else if(operation == "ADD" || operation == "SUB"
            || operation == "MUL" || operation == "CMP"){
            std::string left_token, right_token;
            std::cin >> left_token >> right_token;
            const ExactInteger left = parse_hex(left_token);
            const ExactInteger right = parse_hex(right_token);
            if(operation == "ADD"){
                std::cout << left + right << '\n';
            }else if(operation == "SUB"){
                std::cout << left - right << '\n';
            }else if(operation == "MUL"){
                std::cout << left * right << '\n';
            }else{
                const auto order = left <=> right;
                std::cout << (order < 0 ? -1 : order > 0 ? 1 : 0) << '\n';
            }
        }else if(operation == "MUL_POWER"){
            std::size_t bits;
            std::cin >> bits;
            const ExactInteger power = ExactInteger(1) << bits;
            const ExactInteger product = (power + 1) * (power - 1);
            constexpr std::uint64_t modulus = 1'000'000'007;
            const auto [unused_quotient, remainder] =
                product.divmod(modulus);
            static_cast<void>(unused_quotient);
            std::cout << product.bit_length() << ' ' << remainder
                      << '\n';
        }else if(operation == "SHL" || operation == "SHR"){
            std::string token;
            std::size_t shift;
            std::cin >> token >> shift;
            const ExactInteger value = parse_hex(token);
            std::cout << (operation == "SHL" ? value << shift : value >> shift)
                      << '\n';
        }else if(operation == "ADD_SMALL"){
            std::string token;
            std::uint64_t increment;
            std::cin >> token >> increment;
            ExactInteger value = parse_hex(token);
            value += increment;
            std::cout << value << '\n';
        }else if(operation == "DIV"){
            std::string token;
            std::uint64_t divisor;
            std::cin >> token >> divisor;
            const auto [quotient, remainder] = parse_hex(token).divmod(divisor);
            std::cout << quotient << ' ' << remainder << '\n';
        }else if(operation == "MUL_CHECK"){
            std::string left_token, right_token;
            std::cin >> left_token >> right_token;
            print_product_fingerprint(
                parse_hex(left_token) * parse_hex(right_token)
            );
        }else if(operation == "DENSE_SQUARE"){
            std::size_t bits;
            std::cin >> bits;
            if(bits > (std::numeric_limits<std::size_t>::max)() / 2){
                return 2;
            }
            const ExactInteger value = (ExactInteger(1) << bits) - 1;
            const ExactInteger expected =
                (ExactInteger(1) << (bits * 2))
                - (ExactInteger(1) << (bits + 1)) + 1;
            const ExactInteger product = value * value;
            if(product != expected){
                throw std::runtime_error(
                    "ExactInteger dense-square identity failed"
                );
            }
            ExactInteger aliased = value;
            aliased *= aliased;
            if(aliased != expected || (-value) * value != -expected){
                throw std::runtime_error(
                    "ExactInteger signed or aliased square failed"
                );
            }
            print_product_fingerprint(product);
        }else if(operation == "CONVERT"){
            std::string type, token;
            std::cin >> type >> token;
            const ExactInteger value = parse_hex(token);
            if(type == "I8") print_conversion<std::int8_t>(value);
            else if(type == "U8") print_conversion<std::uint8_t>(value);
            else if(type == "I16") print_conversion<std::int16_t>(value);
            else if(type == "U16") print_conversion<std::uint16_t>(value);
            else if(type == "I32") print_conversion<std::int32_t>(value);
            else if(type == "U32") print_conversion<std::uint32_t>(value);
            else if(type == "I64") print_conversion<std::int64_t>(value);
            else if(type == "U64") print_conversion<std::uint64_t>(value);
            else if(type == "BOOL") print_conversion<bool>(value);
#if defined(__SIZEOF_INT128__)
            else if(type == "I128") print_conversion<__int128_t>(value);
            else if(type == "U128") print_conversion<__uint128_t>(value);
#endif
            else return 2;
        }else{
            return 2;
        }
    }
    return 0;
}
