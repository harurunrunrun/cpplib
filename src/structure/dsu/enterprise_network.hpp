#pragma once

#include <cstdlib>
#include <stdexcept>
#include <vector>

template<int MAX_SIZE>
struct EnterpriseNetwork{
private:
    int _n;
    std::vector<int> parent;
    std::vector<int> distance_to_parent;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex) throw std::runtime_error(message);
    }

public:
    explicit EnterpriseNetwork(int n):
        _n(n), parent(static_cast<std::size_t>(n)),
        distance_to_parent(static_cast<std::size_t>(n), 0){
        if(n < 0 || MAX_SIZE < n) throw std::runtime_error("library assertion fault: range violation (constructor).");
        for(int vertex = 0; vertex < n; ++vertex) parent[static_cast<std::size_t>(vertex)] = vertex;
    }

    void connect(int enterprise, int destination){
        check_vertex(enterprise, "library assertion fault: range violation (connect).");
        check_vertex(destination, "library assertion fault: range violation (connect).");
        parent[static_cast<std::size_t>(enterprise)] = destination;
        distance_to_parent[static_cast<std::size_t>(enterprise)] = std::abs(enterprise - destination) % 1000;
    }

    int distance_to_center(int enterprise){
        check_vertex(enterprise, "library assertion fault: range violation (distance_to_center).");
        int current = enterprise;
        int total = 0;
        std::vector<int> path;
        while(parent[static_cast<std::size_t>(current)] != current){
            path.push_back(current);
            total += distance_to_parent[static_cast<std::size_t>(current)];
            current = parent[static_cast<std::size_t>(current)];
        }
        const int root = current;
        int distance = 0;
        for(auto iterator = path.rbegin(); iterator != path.rend(); ++iterator){
            distance += distance_to_parent[static_cast<std::size_t>(*iterator)];
            parent[static_cast<std::size_t>(*iterator)] = root;
            distance_to_parent[static_cast<std::size_t>(*iterator)] = distance;
        }
        return total;
    }
};
