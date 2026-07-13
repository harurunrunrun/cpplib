#define PROBLEM "https://br.spoj.com/SPOJ/problems/QTREE5/"

#include <iostream>

#include "../../src/structure/tree/dynamic_nearest_marked_vertex.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    DynamicNearestMarkedVertex<int, 100000> nearest(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        nearest.add_edge(left - 1, right - 1, 1);
    }
    nearest.build();

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int operation, vertex;
        std::cin >> operation >> vertex;
        --vertex;
        if(operation == 0){
            nearest.toggle(vertex);
        }else{
            const auto answer = nearest.nearest_distance(vertex);
            std::cout << (answer ? *answer : -1) << '\n';
        }
    }
}
