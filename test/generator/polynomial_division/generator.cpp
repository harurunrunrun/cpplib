// competitive-verifier: DISPLAY hidden

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace{

constexpr std::int64_t MOD = 998244353;
using polynomial = std::vector<std::int64_t>;

void trim(polynomial& values){
    while(!values.empty() && values.back() == 0) values.pop_back();
}

std::int64_t power_mod(std::int64_t base, std::int64_t exponent){
    std::int64_t result = 1;
    while(exponent != 0){
        if((exponent & 1) != 0) result = result * base % MOD;
        base = base * base % MOD;
        exponent >>= 1;
    }
    return result;
}

std::pair<polynomial, polynomial> naive_divmod(
    polynomial dividend,
    polynomial divisor
){
    trim(dividend);
    trim(divisor);
    if(divisor.empty()) throw std::invalid_argument("zero divisor");
    if(dividend.size() < divisor.size()) return {{}, std::move(dividend)};
    polynomial quotient(dividend.size() - divisor.size() + 1);
    const std::int64_t inverse_leading = power_mod(divisor.back(), MOD - 2);
    for(std::size_t remaining = quotient.size(); remaining > 0; --remaining){
        const std::size_t shift = remaining - 1;
        const std::int64_t factor =
            dividend[shift + divisor.size() - 1] * inverse_leading % MOD;
        quotient[shift] = factor;
        for(std::size_t index = 0; index < divisor.size(); ++index){
            dividend[shift + index] =
                (dividend[shift + index] - factor * divisor[index]) % MOD;
            if(dividend[shift + index] < 0) dividend[shift + index] += MOD;
        }
    }
    dividend.resize(divisor.size() - 1);
    trim(quotient);
    trim(dividend);
    return {std::move(quotient), std::move(dividend)};
}

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

void write_coefficients(std::ostream& stream, const polynomial& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) stream << ' ';
        stream << values[index];
    }
    stream << '\n';
}

struct Query{
    polynomial dividend;
    polynomial divisor;
};

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);

    std::vector<Query> queries{
        {{}, {1}},
        {{3, 4}, {7}},
        {{1, 2}, {1, 0, 1}},
        {{0, 0, 5}, {0, 1}}
    };
    std::mt19937_64 engine(0x243f6a8885a308d3ULL);
    for(int iteration = 0; iteration < 180; ++iteration){
        const std::size_t dividend_size = engine() % 70;
        const std::size_t divisor_size = 1 + engine() % 28;
        Query query{polynomial(dividend_size), polynomial(divisor_size)};
        for(auto& value: query.dividend) value = static_cast<std::int64_t>(engine() % MOD);
        for(auto& value: query.divisor) value = static_cast<std::int64_t>(engine() % MOD);
        query.divisor.back() = 1 + static_cast<std::int64_t>(engine() % (MOD - 1));
        queries.push_back(std::move(query));
    }
    Query large{polynomial(300), polynomial(120)};
    for(auto& value: large.dividend) value = static_cast<std::int64_t>(engine() % MOD);
    for(auto& value: large.divisor) value = static_cast<std::int64_t>(engine() % MOD);
    large.divisor.back() = 11;
    queries.push_back(std::move(large));

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(const Query& query: queries){
        input << query.dividend.size() << ' ' << query.divisor.size() << '\n';
        write_coefficients(input, query.dividend);
        write_coefficients(input, query.divisor);
        const auto expected = naive_divmod(query.dividend, query.divisor);
        output << expected.first.size() << ' ' << expected.second.size() << '\n';
        write_coefficients(output, expected.first);
        write_coefficients(output, expected.second);
    }
}
