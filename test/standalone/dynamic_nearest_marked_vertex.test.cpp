// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "../../src/structure/tree/dynamic_nearest_marked_vertex.hpp"

namespace{

using Nearest = DynamicNearestMarkedVertex<long long, 256>;

void test_contract(){
    Nearest empty(0);
    empty.build();
    assert(empty.size() == 0);
    assert(empty.marked_count() == 0);

    Nearest single(1);
    bool thrown = false;
    try{
        (void)single.nearest_distance(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    single.build();
    assert(!single.nearest_distance(0));
    single.mark(0);
    assert(single.nearest_distance(0) == 0);
    single.mark(0);
    assert(single.marked_count() == 1);
    single.unmark(0);
    single.unmark(0);
    assert(single.marked_count() == 0);

    Nearest path(4);
    path.add_edge(0, 1, 4);
    path.add_edge(1, 2, 0);
    path.add_edge(2, 3, 7);
    path.build();
    path.mark(0);
    path.mark(3);
    assert(path.nearest_distance(1) == 4);
    assert(path.nearest_distance(2) == 4);
    path.toggle(0);
    assert(path.nearest_distance(1) == 7);
    path.set_marked(2, true);
    assert(path.nearest_distance(1) == 0);

    thrown = false;
    try{
        Nearest invalid(2);
        invalid.add_edge(0, 1, -1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        Nearest disconnected(2);
        disconnected.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    Nearest overflow(3);
    constexpr long long half = std::numeric_limits<long long>::max() / 2 + 1;
    overflow.add_edge(0, 1, half);
    overflow.add_edge(0, 2, half);
    overflow.build();
    overflow.mark(1);
    thrown = false;
    try{
        (void)overflow.nearest_distance(2);
    }catch(const std::overflow_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        single.mark(1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    test_contract();

    int test_count;
    if(!(std::cin >> test_count)){
        return 0;
    }
    while(test_count--){
        int n, query_count, initially_marked;
        std::cin >> n >> query_count >> initially_marked;
        Nearest nearest(n);
        for(int edge = 0; edge + 1 < n; ++edge){
            int left, right;
            long long weight;
            std::cin >> left >> right >> weight;
            nearest.add_edge(left, right, weight);
        }
        nearest.build(initially_marked != 0);

        while(query_count--){
            char operation;
            int vertex;
            std::cin >> operation >> vertex;
            if(operation == 'T'){
                nearest.toggle(vertex);
            }else if(operation == 'M'){
                nearest.mark(vertex);
            }else if(operation == 'U'){
                nearest.unmark(vertex);
            }else if(operation == 'Q'){
                auto answer = nearest.nearest_distance(vertex);
                if(answer){
                    std::cout << *answer << '\n';
                }else{
                    std::cout << "NONE\n";
                }
            }else{
                assert(operation == 'S');
                std::cout << nearest.is_marked(vertex) << ' '
                          << nearest.marked_count() << '\n';
            }
        }
    }
}
