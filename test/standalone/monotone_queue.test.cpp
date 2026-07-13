// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/structure/other/monotone_queue.hpp"

template<class T>
void print_vector(const std::vector<T>& values){
    for(int i = 0; i < static_cast<int>(values.size()); i++){
        if(i) std::cout << ' ';
        std::cout << values[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
}

void self_test(){
    std::vector<int> values{4, 2, 2, 5, 1};
    assert((sliding_window_min<int, 8>(values, 3) == std::vector<int>{2, 2, 1}));
    assert((sliding_window_max<int, 8>(values, 3) == std::vector<int>{4, 5, 5}));

    MonotoneMinQueue<int, 8> queue;
    queue.push(0, 3);
    queue.push(1, 3);
    assert(queue.best() == 3);
    assert(queue.best_index() == 1);
    queue.push(2, 4);
    queue.expire(2);
    assert(queue.best() == 4);
    assert(queue.best_index() == 2);
    queue.pop_front();
    assert(queue.empty());
    queue.clear();
    assert(queue.empty());
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)){
        self_test();
        return 0;
    }
    while(test_count--){
        int n, window;
        std::cin >> n >> window;
        std::vector<int> values(static_cast<std::size_t>(n));
        for(int& value: values) std::cin >> value;

        print_vector(sliding_window_min<int, 512>(values, window));
        print_vector(sliding_window_max<int, 512>(values, window));

        MonotoneMinQueue<int, 512> queue;
        std::vector<long long> best_indices;
        for(int i = 0; i < n; i++){
            queue.expire(static_cast<long long>(i) - window + 1);
            queue.push(i, values[static_cast<std::size_t>(i)]);
            if(i + 1 >= window) best_indices.push_back(queue.best_index());
        }
        print_vector(best_indices);
    }
}
