// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree/count_directed_spanning_arborescences.hpp"

namespace{

template<class T>
T brute_count(const std::vector<std::vector<T>>& adjacency){
    const int vertex_count = static_cast<int>(adjacency.size());
    T answer{};
    for(int root = 0; root < vertex_count; ++root){
        std::vector<std::vector<int>> candidates(
            static_cast<std::size_t>(vertex_count)
        );
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(vertex == root) continue;
            for(int parent = 0; parent < vertex_count; ++parent){
                if(parent != vertex
                   && adjacency[static_cast<std::size_t>(parent)]
                               [static_cast<std::size_t>(vertex)] != T{}){
                    candidates[static_cast<std::size_t>(vertex)].push_back(parent);
                }
            }
        }
        std::vector<int> parent(static_cast<std::size_t>(vertex_count), -1);
        parent[static_cast<std::size_t>(root)] = root;
        auto enumerate = [&](auto&& self, int vertex, T product) -> void {
            if(vertex == vertex_count){
                for(int start = 0; start < vertex_count; ++start){
                    int current = start;
                    std::vector<char> seen(
                        static_cast<std::size_t>(vertex_count), 0
                    );
                    while(current != root
                          && current != -1
                          && !seen[static_cast<std::size_t>(current)]){
                        seen[static_cast<std::size_t>(current)] = 1;
                        current = parent[static_cast<std::size_t>(current)];
                    }
                    if(current != root) return;
                }
                answer += product;
                return;
            }
            if(vertex == root){
                self(self, vertex + 1, product);
                return;
            }
            for(const int next_parent:
                candidates[static_cast<std::size_t>(vertex)]){
                parent[static_cast<std::size_t>(vertex)] = next_parent;
                self(
                    self,
                    vertex + 1,
                    product * adjacency[static_cast<std::size_t>(next_parent)]
                                       [static_cast<std::size_t>(vertex)]
                );
            }
            parent[static_cast<std::size_t>(vertex)] = -1;
        };
        enumerate(enumerate, 0, T(1));
    }
    return answer;
}

std::vector<std::vector<long long>> binary_weights(
    const std::vector<std::string>& adjacency
){
    std::vector<std::vector<long long>> weights(
        adjacency.size(), std::vector<long long>(adjacency.size())
    );
    for(std::size_t from = 0; from < adjacency.size(); ++from){
        for(std::size_t to = 0; to < adjacency.size(); ++to){
            weights[from][to] = adjacency[from][to] == '1';
        }
    }
    return weights;
}

void check_binary(const std::vector<std::string>& adjacency){
    assert(
        count_directed_spanning_arborescences(adjacency)
        == brute_count(binary_weights(adjacency))
    );
}

template<int MOD>
void check_weighted(const std::vector<std::vector<long long>>& adjacency){
    using Mint = Modint<MOD>;
    std::vector<std::vector<Mint>> field(
        adjacency.size(), std::vector<Mint>(adjacency.size())
    );
    for(std::size_t from = 0; from < adjacency.size(); ++from){
        for(std::size_t to = 0; to < adjacency.size(); ++to){
            field[from][to] = Mint(adjacency[from][to]);
        }
    }
    const Mint expected = brute_count(field);
    assert(count_directed_spanning_arborescences(field) == expected);
    assert(
        count_directed_spanning_arborescences_mod<MOD>(adjacency)
        == expected.val()
    );
}

void self_test(){
    check_binary({"0"});
    check_binary({"1"});
    check_binary({"0100", "0010", "0001", "0000"});
    check_binary({"0100", "0010", "0001", "1000"});
    check_binary({"010", "001", "000"});
    check_binary({"000", "000", "000"});

    std::mt19937 random(20260717);
    for(int vertex_count = 1; vertex_count <= 6; ++vertex_count){
        for(int iteration = 0; iteration < 120; ++iteration){
            std::vector<std::string> adjacency(
                static_cast<std::size_t>(vertex_count),
                std::string(static_cast<std::size_t>(vertex_count), '0')
            );
            for(auto& row: adjacency){
                for(char& edge: row){
                    edge = (random() % 4 == 0 ? '1' : '0');
                }
            }
            check_binary(adjacency);
        }
    }

    constexpr int modulus = 998244353;
    check_weighted<modulus>({{0}});
    check_weighted<modulus>({{91}});
    check_weighted<modulus>({{0, 2}, {3, 0}});
    check_weighted<modulus>({{7, -2, 0}, {5, 11, 3}, {4, 0, -9}});
    for(int vertex_count = 1; vertex_count <= 5; ++vertex_count){
        for(int iteration = 0; iteration < 160; ++iteration){
            std::vector<std::vector<long long>> adjacency(
                static_cast<std::size_t>(vertex_count),
                std::vector<long long>(static_cast<std::size_t>(vertex_count))
            );
            for(auto& row: adjacency){
                for(long long& weight: row){
                    weight = random() % 4 == 0
                        ? 0
                        : static_cast<long long>(random() % 11) - 5;
                }
            }
            check_weighted<modulus>(adjacency);
        }
    }

    std::vector<std::vector<Modint<5>>> complete_mod_five(
        5, std::vector<Modint<5>>(5, Modint<5>(1))
    );
    assert(count_directed_spanning_arborescences(complete_mod_five).val() == 0);
    std::vector<std::vector<long long>> complete_five(
        5, std::vector<long long>(5, 1)
    );
    assert(count_directed_spanning_arborescences_mod<5>(complete_five) == 0);

    std::vector<std::vector<long long>> chain(
        128, std::vector<long long>(128)
    );
    for(int vertex = 0; vertex + 1 < 128; ++vertex){
        chain[static_cast<std::size_t>(vertex)]
             [static_cast<std::size_t>(vertex + 1)] = 1;
    }
    assert(count_directed_spanning_arborescences_mod<modulus>(chain) == 1);

    const std::vector<std::vector<unsigned long long>> unsigned_weights{
        {0, 998244354ULL}, {998244355ULL, 0}
    };
    assert(
        count_directed_spanning_arborescences_mod<modulus>(unsigned_weights)
        == 3
    );

    std::vector<std::string> complete(8, std::string(8, '1'));
    assert(count_directed_spanning_arborescences(complete) == 2097152);

    for(const auto& invalid: std::vector<std::vector<std::string>>{
        {},
        std::vector<std::string>(9, std::string(9, '0')),
        {"00", "0"},
        {"0x", "00"},
    }){
        bool thrown = false;
        try{
            (void)count_directed_spanning_arborescences(invalid);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    for(const auto& invalid:
        std::vector<std::vector<std::vector<long long>>>{
            {},
            {{0, 1}, {0}},
        }){
        bool thrown = false;
        try{
            (void)count_directed_spanning_arborescences_mod<modulus>(invalid);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
}

template<int MOD>
void solve_weighted(){
    int vertex_count;
    std::cin >> vertex_count;
    std::vector<std::vector<long long>> adjacency(
        static_cast<std::size_t>(vertex_count),
        std::vector<long long>(static_cast<std::size_t>(vertex_count))
    );
    for(auto& row: adjacency){
        for(long long& weight: row) std::cin >> weight;
    }
    std::cout << count_directed_spanning_arborescences_mod<MOD>(adjacency)
              << '\n';
}

}  // namespace

int main(){
    std::string mode;
    if(!(std::cin >> mode)){
        self_test();
        return 0;
    }
    if(mode == "WEIGHTED"){
        solve_weighted<998244353>();
        return 0;
    }
    if(mode == "WEIGHTED5"){
        solve_weighted<5>();
        return 0;
    }

    int vertex_count;
    if(mode == "BINARY"){
        std::cin >> vertex_count;
    }else{
        vertex_count = std::stoi(mode);
    }
    std::vector<std::string> adjacency(
        static_cast<std::size_t>(vertex_count)
    );
    for(auto& row: adjacency) std::cin >> row;
    std::cout << count_directed_spanning_arborescences(adjacency) << '\n';
}
