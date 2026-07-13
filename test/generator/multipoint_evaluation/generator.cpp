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

std::int64_t evaluate(const polynomial& coefficients, std::int64_t point){
    std::int64_t result = 0;
    for(std::size_t index = coefficients.size(); index > 0; --index){
        result = (result * point + coefficients[index - 1]) % MOD;
    }
    return result;
}

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

void write_values(std::ostream& stream, const std::vector<std::int64_t>& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) stream << ' ';
        stream << values[index];
    }
    stream << '\n';
}

struct Query{
    polynomial coefficients;
    std::vector<std::int64_t> points;
};

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);

    std::vector<Query> queries{
        {{}, {}},
        {{}, {0, 1}},
        {{2, 3, 4}, {0, 1, 5, 5}},
        {{7}, {3, 9, 27}}
    };
    std::mt19937_64 engine(0x452821e638d01377ULL);
    for(int iteration = 0; iteration < 220; ++iteration){
        Query query{
            polynomial(engine() % 55),
            std::vector<std::int64_t>(engine() % 55)
        };
        for(auto& value: query.coefficients) value = static_cast<std::int64_t>(engine() % MOD);
        for(auto& value: query.points) value = static_cast<std::int64_t>(engine() % MOD);
        queries.push_back(std::move(query));
    }
    Query large{polynomial(260), std::vector<std::int64_t>(255)};
    for(auto& value: large.coefficients) value = static_cast<std::int64_t>(engine() % MOD);
    for(auto& value: large.points) value = static_cast<std::int64_t>(engine() % MOD);
    queries.push_back(std::move(large));

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(const Query& query: queries){
        input << query.coefficients.size() << ' ' << query.points.size() << '\n';
        write_values(input, query.coefficients);
        write_values(input, query.points);
        std::vector<std::int64_t> expected;
        expected.reserve(query.points.size());
        for(const auto point: query.points){
            expected.push_back(evaluate(query.coefficients, point));
        }
        write_values(output, expected);
    }
}
