// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/euler_tour.hpp"

void self_test(){
    TreeEulerTour<8> tour(6);
    tour.add_edge(0, 1);
    tour.add_edge(0, 2);
    tour.add_edge(1, 3);
    tour.add_edge(1, 4);
    tour.add_edge(2, 5);
    tour.build(0);

    assert((tour.preorder() == std::vector<int>{0, 1, 3, 4, 2, 5}));
    assert(tour.parent(0) == -1);
    assert(tour.parent(4) == 1);
    assert(tour.depth(5) == 2);
    assert((tour.subtree_range(1) == std::pair<int, int>{1, 4}));
    assert(tour.subtree_size(2) == 2);
    assert(tour.is_ancestor(0, 5));
    assert(!tour.is_ancestor(1, 5));

    const auto events = tour.euler_tour();
    assert(events.size() == 12);
    assert(events.front().vertex == 0 && events.front().entering);
    assert(events.back().vertex == 0 && !events.back().entering);
    for(int vertex = 0; vertex < tour.size(); ++vertex){
        assert(tour.preorder_vertex(tour.tin(vertex)) == vertex);
    }

    TreeEulerTour<0> empty;
    empty.build();
    assert(empty.root() == -1);
    assert(empty.preorder().empty());
    assert(empty.euler_tour().empty());
}

int main(){
    int n, root;
    if(!(std::cin >> n >> root)){
        self_test();
        return 0;
    }

    TreeEulerTour<256> tour(n);
    for(int edge = 0; edge + 1 < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tour.add_edge(u, v);
    }
    tour.build(root);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "PARENT"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.parent(vertex) << '\n';
        }else if(type == "DEPTH"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.depth(vertex) << '\n';
        }else if(type == "TIN"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.tin(vertex) << '\n';
        }else if(type == "TOUT"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.tout(vertex) << '\n';
        }else if(type == "RANGE"){
            int vertex;
            std::cin >> vertex;
            const auto [left, right] = tour.subtree_range(vertex);
            std::cout << left << ' ' << right << '\n';
        }else if(type == "SUBSIZE"){
            int vertex;
            std::cin >> vertex;
            std::cout << tour.subtree_size(vertex) << '\n';
        }else if(type == "ANCESTOR"){
            int ancestor, descendant;
            std::cin >> ancestor >> descendant;
            std::cout << tour.is_ancestor(ancestor, descendant) << '\n';
        }else if(type == "PREORDER_VERTEX"){
            int index;
            std::cin >> index;
            std::cout << tour.preorder_vertex(index) << '\n';
        }else if(type == "PREORDER"){
            const auto order = tour.preorder();
            std::cout << order.size();
            for(int vertex : order) std::cout << ' ' << vertex;
            std::cout << '\n';
        }else if(type == "EULER_SIZE"){
            std::cout << tour.euler_size() << '\n';
        }else if(type == "EVENT"){
            int index;
            std::cin >> index;
            const auto event = tour.euler_event(index);
            std::cout
                << event.vertex << ' '
                << event.entering << '\n';
        }else if(type == "EULER"){
            const auto events = tour.euler_tour();
            std::cout << events.size();
            for(const auto event : events){
                std::cout
                    << ' ' << event.vertex
                    << ' ' << event.entering;
            }
            std::cout << '\n';
        }else if(type == "ROOT"){
            std::cout << tour.root() << '\n';
        }else if(type == "SIZE"){
            std::cout << tour.size() << '\n';
        }else if(type == "EDGES"){
            std::cout << tour.edge_count() << '\n';
        }
    }
}
