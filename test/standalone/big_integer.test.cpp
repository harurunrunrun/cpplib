// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/big_integer.hpp"

static void test_parse_and_arithmetic(){
    assert(BigInteger("0").to_string() == "0");
    assert(BigInteger("+00000123").to_string() == "123");
    assert(BigInteger("-00000123").to_string() == "-123");
    const BigInteger power = BigInteger(1) << 500;
    assert((power >> 499) == 2);
    assert((power * power).bit_length() == 1001);
    assert(BigInteger("123456789012345678901234567890")
         + BigInteger("987654321098765432109876543210")
         == BigInteger("1111111110111111111011111111100"));
    assert(BigInteger("12345678901234567890")
         * BigInteger("98765432109876543210")
         == BigInteger("1219326311370217952237463801111263526900"));

    bool thrown = false;
    try{ (void)BigInteger(""); }catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)BigInteger("+"); }catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)BigInteger("12x"); }catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
}

static void test_division_signs(){
    for(long long dividend = -200; dividend <= 200; ++dividend){
        for(long long divisor = -40; divisor <= 40; ++divisor){
            if(divisor == 0) continue;
            const BigInteger a(dividend);
            const BigInteger b(divisor);
            const auto [quotient, remainder] = BigInteger::divmod(a, b);
            assert(quotient == dividend / divisor);
            assert(remainder == dividend % divisor);
            assert(a == quotient * b + remainder);
            assert(abs(remainder) < abs(b));
        }
    }
    const BigInteger large("1234567890123456789012345678901234567890");
    const BigInteger divisor("98765432109876543210");
    const auto [quotient, remainder] = BigInteger::divmod(large, divisor);
    assert(quotient == BigInteger("12499999886093750001"));
    assert(remainder == BigInteger("54205246805420524680"));
    assert(large == quotient * divisor + remainder);
    bool thrown = false;
    try{ (void)(large / BigInteger(0)); }
    catch(const std::domain_error&){ thrown = true; }
    assert(thrown);
}

static void test_random_native(){
    std::mt19937_64 rng(2026071615);
    for(int iteration = 0; iteration < 200000; ++iteration){
        const std::int64_t left = static_cast<std::int64_t>(rng() % 2'000'000'001ULL)
            - 1'000'000'000LL;
        std::int64_t right = static_cast<std::int64_t>(rng() % 2'000'001ULL)
            - 1'000'000LL;
        if(right == 0) right = 1;
        const BigInteger a(left), b(right);
        assert((a + b).checked_to<std::int64_t>() == left + right);
        assert((a - b).checked_to<std::int64_t>() == left - right);
        assert((a * b).checked_to<std::int64_t>() == left * right);
        assert((a / b).checked_to<std::int64_t>() == left / right);
        assert((a % b).checked_to<std::int64_t>() == left % right);
    }
}

static void test_increment_and_stream(){
    BigInteger value(-1);
    assert(value++ == -1 && value == 0);
    assert(++value == 1);
    assert(value-- == 1 && value == 0);
    assert(--value == -1);
    std::stringstream stream("-123456789012345678901 77x 9");
    BigInteger parsed;
    stream >> parsed;
    assert(parsed == BigInteger("-123456789012345678901"));
    stream >> parsed;
    assert(stream.fail());
    assert(parsed == BigInteger("-123456789012345678901"));
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        test_parse_and_arithmetic();
        test_division_signs();
        test_random_native();
        test_increment_and_stream();
        return 0;
    }
    while(query_count-- > 0){
        std::string operation, left_text;
        std::cin >> operation >> left_text;
        const BigInteger left(left_text);
        if(operation == "ECHO"){
            std::cout << left << '\n';
            continue;
        }
        std::string right_text;
        std::cin >> right_text;
        const BigInteger right(right_text);
        if(operation == "ADD") std::cout << left + right << '\n';
        else if(operation == "SUB") std::cout << left - right << '\n';
        else if(operation == "MUL") std::cout << left * right << '\n';
        else if(operation == "DIV") std::cout << left / right << '\n';
        else if(operation == "MOD") std::cout << left % right << '\n';
        else if(operation == "CMP") std::cout << (left < right ? -1 : left > right) << '\n';
        else if(operation == "SHL"){
            std::cout << (left << right.checked_to<std::size_t>()) << '\n';
        }else if(operation == "SHR"){
            std::cout << (left >> right.checked_to<std::size_t>()) << '\n';
        }else{
            throw std::runtime_error("unknown BigInteger operation");
        }
    }
}

