// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/randomized/color_coding.hpp"
#include "../../src/approximate/randomized/gibbs_sampling.hpp"

namespace {

template<class Exception, class Function>
bool throws_as(Function&& function){
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        return true;
    }catch(...){
    }
    return false;
}

bool brute_path(
    const std::vector<std::vector<std::size_t>>& graph,
    std::size_t required
){
    if(required == 0) return true;
    const auto visit = [&](auto&& self, std::size_t vertex,
                           std::vector<unsigned char>& used,
                           std::size_t length) -> bool {
        if(length == required) return true;
        for(const std::size_t to: graph[vertex]){
            if(used[to] != 0) continue;
            used[to] = 1;
            if(self(self, to, used, length + 1)) return true;
            used[to] = 0;
        }
        return false;
    };
    std::vector<unsigned char> used(graph.size(), 0);
    for(std::size_t start = 0; start < graph.size(); ++start){
        used[start] = 1;
        if(visit(visit, start, used, 1)) return true;
        used[start] = 0;
    }
    return false;
}

bool boundary_tests(){
    using namespace approximate::randomized;
    std::mt19937_64 rng(7);
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(color_coding_simple_path(
               std::vector<std::vector<std::size_t>>{{1}}, 1, 1, rng
           ));
       })) return false;
    const std::size_t digits = std::numeric_limits<std::size_t>::digits;
    if(!throws_as<std::length_error>([&]{
           static_cast<void>(color_coding_simple_path(
               std::vector<std::vector<std::size_t>>(digits), digits, 1, rng
           ));
       })) return false;
    const auto empty = color_coding_simple_path(
        std::vector<std::vector<std::size_t>>{}, 0, 0, rng
    );
    if(!empty.found || !empty.path.empty() || empty.trials != 0) return false;
    const auto one = color_coding_simple_path(
        std::vector<std::vector<std::size_t>>(2), 1, 0, rng
    );
    if(!one.found || one.path != std::vector<std::size_t>{0}) return false;
    const auto impossible = color_coding_simple_path(
        std::vector<std::vector<std::size_t>>(2), 3, 10, rng
    );
    if(impossible.found || impossible.trials != 0) return false;
    auto corrupt = one;
    corrupt.path = {2};
    if(is_valid_color_coding_path(
           std::vector<std::vector<std::size_t>>(2), 1, corrupt
       )) return false;
    if(!throws_as<std::invalid_argument>([&]{
           validate_color_coding_path(
               std::vector<std::vector<std::size_t>>(2), 1, corrupt
           );
       })) return false;

    const auto sampler = [](
        std::size_t coordinate,
        std::span<const int> state,
        std::mt19937_64&
    ){
        return coordinate == 0 ? 1 : state[coordinate - 1];
    };
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(gibbs_sample(
               std::vector<int>{0}, sampler, 0, 0, 1, rng
           ));
       })) return false;
    if(!throws_as<std::overflow_error>([&]{
           static_cast<void>(gibbs_sample(
               std::vector<int>{}, sampler,
               std::numeric_limits<std::size_t>::max(), 2,
               std::numeric_limits<std::size_t>::max(), rng
           ));
       })) return false;
    const auto none = gibbs_sample(
        std::vector<int>{0}, sampler, 0, 0, 0, rng
    );
    return none.empty();
}

bool gibbs_tests(){
    using approximate::randomized::gibbs_sample;
    std::mt19937_64 rng(19);
    std::size_t calls = 0;
    const auto samples = gibbs_sample(
        std::vector<int>{0, 0, 0},
        [&calls](std::size_t coordinate, std::span<const int> state,
                 std::mt19937_64&){
            ++calls;
            return coordinate == 0 ? 1 : state[coordinate - 1];
        },
        2, 3, 4, rng
    );
    if(calls != (2 + 3 * 4) * 3 || samples.size() != 4) return false;
    return std::all_of(samples.begin(), samples.end(), [](const auto& sample){
        return sample == std::vector<int>{1, 1, 1};
    });
}

}  // namespace

int main(){
    using namespace approximate::randomized;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    if(!boundary_tests() || !gibbs_tests()) return 2;

    std::size_t case_count = 0;
    std::cin >> case_count;
    std::size_t found_count = 0;
    for(std::size_t index = 0; index < case_count; ++index){
        std::size_t n = 0;
        std::size_t m = 0;
        std::size_t required = 0;
        unsigned int expected_input = 0;
        std::cin >> n >> m >> required >> expected_input;
        std::vector<std::vector<std::size_t>> graph(n);
        for(std::size_t edge = 0; edge < m; ++edge){
            std::size_t from = 0;
            std::size_t to = 0;
            std::cin >> from >> to;
            graph[from].push_back(to);
        }
        const bool expected = expected_input != 0;
        if(brute_path(graph, required) != expected) return 3;
        std::mt19937_64 rng(0xC010C0D1ULL + index);
        const auto result = color_coding_simple_path(
            graph, required, 2000, rng
        );
        if(result.found != expected) return 4;
        if(!is_valid_color_coding_path(graph, required, result)) return 5;
        validate_color_coding_path(graph, required, result);
        found_count += static_cast<std::size_t>(result.found);
    }
    std::cout << "OK " << case_count << ' ' << found_count << '\n';
}
