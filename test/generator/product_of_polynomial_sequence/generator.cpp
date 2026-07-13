// competitive-verifier: DISPLAY hidden

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

polynomial naive_multiply(const polynomial& left, const polynomial& right){
    if(left.empty() || right.empty()) return {};
    polynomial result(left.size() + right.size() - 1);
    for(std::size_t i = 0; i < left.size(); ++i){
        for(std::size_t j = 0; j < right.size(); ++j){
            result[i + j] = (
                result[i + j] + left[i] * right[j]
            ) % MOD;
        }
    }
    trim(result);
    return result;
}

polynomial naive_product(const std::vector<polynomial>& polynomials){
    polynomial result{1};
    for(polynomial factor: polynomials){
        trim(factor);
        result = naive_multiply(result, factor);
    }
    return result;
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

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);

    std::vector<std::vector<polynomial>> queries{
        {},
        {{}},
        {{2}, {1, 2}, {3, 2, 1}},
        {{0, 0}, {1, 1}}
    };
    std::mt19937_64 engine(0x13198a2e03707344ULL);
    for(int iteration = 0; iteration < 180; ++iteration){
        const std::size_t count = engine() % 15;
        std::vector<polynomial> factors(count);
        for(polynomial& factor: factors){
            const std::size_t size = engine() % 10;
            factor.resize(size);
            for(auto& value: factor) value = static_cast<std::int64_t>(engine() % MOD);
        }
        queries.push_back(std::move(factors));
    }
    std::vector<polynomial> large(80);
    for(polynomial& factor: large){
        factor.resize(5);
        for(auto& value: factor) value = static_cast<std::int64_t>(engine() % MOD);
        factor.back() = 1 + static_cast<std::int64_t>(engine() % (MOD - 1));
    }
    queries.push_back(std::move(large));

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(const auto& factors: queries){
        input << factors.size() << '\n';
        for(const polynomial& factor: factors){
            input << factor.size();
            for(const auto value: factor) input << ' ' << value;
            input << '\n';
        }
        const polynomial expected = naive_product(factors);
        output << expected.size() << '\n';
        write_coefficients(output, expected);
    }
}
