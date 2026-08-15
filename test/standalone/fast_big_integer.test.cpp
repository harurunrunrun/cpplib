// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/integer/fast_big_integer.hpp"

static void run_local_contract_tests(){
    assert(BigInteger("0").to_string() == "0");
    assert(BigInteger("+000").to_string() == "0");
    assert(BigInteger("-0").to_string() == "0");
    assert(BigInteger("000001").to_string() == "1");
    assert(BigInteger("-000001").to_string() == "-1");

    const BigInteger a("123456789012345678901234567890");
    const BigInteger b("98765432109876543210");
    const auto [quotient, remainder] = BigInteger::divmod(a, b);
    assert(a == quotient * b + remainder);
    assert(abs(remainder) < abs(b));

    for(long long left = -100; left <= 100; ++left){
        for(long long right = -20; right <= 20; ++right){
            const BigInteger x(left), y(right);
            assert((x + y).checked_to<long long>() == left + right);
            assert((x - y).checked_to<long long>() == left - right);
            assert((x * y).checked_to<long long>() == left * right);
            if(right != 0){
                const auto [q, r] = BigInteger::divmod(x, y);
                assert(q.checked_to<long long>() == left / right);
                assert(r.checked_to<long long>() == left % right);
            }
        }
    }

    BigInteger alias("999999999999999999999999999999999999");
    const BigInteger alias_copy = alias;
    alias += alias;
    assert(alias == alias_copy * 2);
    alias = alias_copy;
    alias *= alias;
    assert(alias == alias_copy * alias_copy);
    alias /= alias_copy;
    assert(alias == alias_copy);
    alias %= alias_copy;
    assert(alias == 0);
    const BigInteger ntt_square(std::string(1200, static_cast<char>(57)));
    const BigInteger ntt_square_copy = ntt_square;
    const BigInteger ntt_square_expected = ntt_square * ntt_square_copy;
    assert(ntt_square * ntt_square == ntt_square_expected);
    BigInteger ntt_compound_square = ntt_square;
    ntt_compound_square *= ntt_compound_square;
    assert(ntt_compound_square == ntt_square_expected);

    assert(BigInteger(std::numeric_limits<std::int64_t>::min())
        .checked_to<std::int64_t>() == std::numeric_limits<std::int64_t>::min());
    assert(BigInteger(std::numeric_limits<std::uint64_t>::max())
        .checked_to<std::uint64_t>() == std::numeric_limits<std::uint64_t>::max());

    assert(BigInteger(true).checked_to<bool>());
    bool thrown = false;
    try{ (void)BigInteger(""); }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)BigInteger("+"); }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)BigInteger("12x"); }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)BigInteger::divmod(a, 0); }
    catch(const std::domain_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)BigInteger("18446744073709551616").checked_to<std::uint64_t>(); }
    catch(const std::overflow_error&){ thrown = true; }
    assert(thrown);

    std::stringstream stream("-123456789012345678901 77x");
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
        run_local_contract_tests();
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
        else if(operation == "DIVMOD"){
            const auto [quotient, remainder] = BigInteger::divmod(left, right);
            std::cout << quotient << ' ' << remainder << '\n';
        }else if(operation == "CMP"){
            std::cout << (left < right ? -1 : left > right) << '\n';
        }else{
            throw std::runtime_error("unknown BigInteger operation");
        }
    }
}
