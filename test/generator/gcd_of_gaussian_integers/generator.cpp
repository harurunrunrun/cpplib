// competitive-verifier: DISPLAY hidden

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace{

struct Gaussian{
    long long real;
    long long imag;
};

long long norm(Gaussian value){
    return value.real * value.real + value.imag * value.imag;
}

bool zero(Gaussian value){
    return value.real == 0 && value.imag == 0;
}

bool divides(Gaussian divisor, Gaussian value){
    if(zero(divisor)) return zero(value);
    const long long denominator = norm(divisor);
    const long long real_numerator = value.real * divisor.real + value.imag * divisor.imag;
    const long long imag_numerator = value.imag * divisor.real - value.real * divisor.imag;
    return real_numerator % denominator == 0 && imag_numerator % denominator == 0;
}

Gaussian canonical(Gaussian value){
    const Gaussian candidates[] = {
        value,
        {-value.imag, value.real},
        {-value.real, -value.imag},
        {value.imag, -value.real},
    };
    for(Gaussian candidate: candidates){
        if(candidate.real > 0 && candidate.imag >= 0) return candidate;
    }
    return {0, 0};
}

Gaussian brute(Gaussian left, Gaussian right){
    if(zero(left) && zero(right)) return {0, 0};
    const long long limit = std::min(
        zero(left) ? norm(right) : norm(left),
        zero(right) ? norm(left) : norm(right)
    );
    const long long component_limit = static_cast<long long>(std::sqrt(limit)) + 1;
    long long best_norm = 0;
    Gaussian best{0, 0};
    for(long long real = -component_limit; real <= component_limit; ++real){
        for(long long imag = -component_limit; imag <= component_limit; ++imag){
            const Gaussian candidate{real, imag};
            const long long candidate_norm = norm(candidate);
            if(candidate_norm == 0 || candidate_norm > limit) continue;
            if(candidate_norm > best_norm && divides(candidate, left) && divides(candidate, right)){
                best_norm = candidate_norm;
                best = canonical(candidate);
            }
        }
    }
    return best;
}

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);
    std::vector<std::pair<Gaussian, Gaussian>> queries{
        {{0, 0}, {0, 0}},
        {{1'000'000'000, 0}, {0, 0}},
        {{1'000'000'000, 1'000'000'000}, {1'000'000'000, -1'000'000'000}},
    };
    std::vector<Gaussian> expected{
        {0, 0},
        {1'000'000'000, 0},
        {1'000'000'000, 1'000'000'000},
    };
    std::mt19937 random(20260713);
    std::uniform_int_distribution<int> component(-20, 20);
    for(int iteration = 0; iteration < 700; ++iteration){
        const Gaussian left{component(random), component(random)};
        const Gaussian right{component(random), component(random)};
        queries.emplace_back(left, right);
        expected.push_back(brute(left, right));
    }

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(std::size_t index = 0; index < queries.size(); ++index){
        input << queries[index].first.real << ' ' << queries[index].first.imag << ' '
              << queries[index].second.real << ' ' << queries[index].second.imag << '\n';
        output << expected[index].real << ' ' << expected[index].imag << '\n';
    }
}
