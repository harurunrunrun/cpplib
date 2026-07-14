// competitive-verifier: STANDALONE

#include <cassert>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/range/sliding_window_maximum.hpp"
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
    assert((sliding_window_maximum(values, 3) == std::vector<int>{4, 5, 5}));

    MonotoneMinQueue<int, 8> queue;
    assert(queue.empty());
    assert(queue.size() == 0);
    queue.push(0, 3);
    queue.push(1, 3);
    assert(queue.size() == 1);
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

    MonotoneMaxQueue<int, 8> maximum;
    maximum.push(0, 1);
    maximum.push(1, 5);
    maximum.push(2, 3);
    assert(maximum.best() == 5);
    assert(maximum.best_index() == 1);
    assert((sliding_window_extreme<int, 8>(
        values, 3, std::greater<int>{}) == std::vector<int>{4, 5, 5}));

    for(int operation = 0; operation < 3; ++operation){
        bool thrown = false;
        try{
            if(operation == 0) queue.pop_front();
            if(operation == 1) (void)queue.best();
            if(operation == 2) (void)queue.best_index();
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }

    MonotoneMinQueue<int, 2> full;
    full.push(0, 1);
    full.push(1, 2);
    bool thrown = false;
    try{ full.push(2, 3); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ full.push(1, 0); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    for(int window: {0, 4, 9}){
        thrown = false;
        try{ (void)sliding_window_min<int, 3>(values, window); }
        catch(const std::runtime_error&){ thrown = true; }
        assert(thrown);
    }
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
        print_vector(sliding_window_maximum(values, window));

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
