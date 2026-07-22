// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree/count_directed_spanning_arborescences.hpp"

long long brute_count(const std::vector<std::string>& adjacency){
    const int vertex_count = static_cast<int>(adjacency.size());
    long long answer = 0;
    for(int root = 0; root < vertex_count; ++root){
        std::vector<std::vector<int>> candidates(
            static_cast<std::size_t>(vertex_count)
        );
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(vertex == root) continue;
            for(int parent = 0; parent < vertex_count; ++parent){
                if(parent != vertex
                   && adjacency[static_cast<std::size_t>(parent)]
                               [static_cast<std::size_t>(vertex)] == '1'){
                    candidates[static_cast<std::size_t>(vertex)].push_back(parent);
                }
            }
        }
        std::vector<int> parent(static_cast<std::size_t>(vertex_count), -1);
        parent[static_cast<std::size_t>(root)] = root;
        auto enumerate = [&](auto&& self, int vertex) -> void {
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
                ++answer;
                return;
            }
            if(vertex == root){
                self(self, vertex + 1);
                return;
            }
            for(const int next_parent:
                candidates[static_cast<std::size_t>(vertex)]){
                parent[static_cast<std::size_t>(vertex)] = next_parent;
                self(self, vertex + 1);
            }
            parent[static_cast<std::size_t>(vertex)] = -1;
        };
        enumerate(enumerate, 0);
    }
    return answer;
}

void check_case(const std::vector<std::string>& adjacency){
    assert(
        count_directed_spanning_arborescences(adjacency)
        == brute_count(adjacency)
    );
}

void self_test(){
    check_case({"0"});
    check_case({"1"});
    check_case({"0100", "0010", "0001", "0000"});
    check_case({"0100", "0010", "0001", "1000"});
    check_case({"010", "001", "000"});
    check_case({"000", "000", "000"});

    std::mt19937 random(20260717);
    for(int vertex_count = 1; vertex_count <= 6; ++vertex_count){
        for(int iteration = 0; iteration < 250; ++iteration){
            std::vector<std::string> adjacency(
                static_cast<std::size_t>(vertex_count),
                std::string(static_cast<std::size_t>(vertex_count), '0')
            );
            for(auto& row: adjacency){
                for(char& edge: row){
                    edge = (random() % 4 == 0 ? '1' : '0');
                }
            }
            check_case(adjacency);
        }
    }

    std::vector<std::string> complete(8, std::string(8, '1'));
    assert(count_directed_spanning_arborescences(complete) == 2097152);
    for(int iteration = 0; iteration < 5000; ++iteration){
        assert(count_directed_spanning_arborescences(complete) == 2097152);
    }

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
}

int main(){
    int vertex_count;
    if(!(std::cin >> vertex_count)){
        self_test();
        return 0;
    }
    std::vector<std::string> adjacency(
        static_cast<std::size_t>(vertex_count)
    );
    for(auto& row: adjacency) std::cin >> row;
    std::cout << count_directed_spanning_arborescences(adjacency) << '\n';
}
