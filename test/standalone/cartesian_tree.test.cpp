// competitive-verifier: STANDALONE

#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/tree/cartesian_tree.hpp"

template<class T, class Compare>
void validate(
    const std::vector<T>& values,
    const CartesianTreeResult& tree,
    Compare compare
){
    const int n = static_cast<int>(values.size());
    assert(tree.size() == n);
    assert(tree.empty() == (n == 0));
    if(n == 0){
        assert(tree.root == -1);
        return;
    }

    assert(0 <= tree.root && tree.root < n);
    assert(tree.parent[static_cast<std::size_t>(tree.root)] == -1);

    std::vector<int> order;
    std::vector<int> stack;
    int current = tree.root;
    while(current != -1 || !stack.empty()){
        while(current != -1){
            stack.push_back(current);
            current = tree.left[static_cast<std::size_t>(current)];
        }
        current = stack.back();
        stack.pop_back();
        order.push_back(current);
        current = tree.right[static_cast<std::size_t>(current)];
    }
    for(int index = 0; index < n; ++index){
        assert(order[static_cast<std::size_t>(index)] == index);
        const int parent = tree.parent[static_cast<std::size_t>(index)];
        if(parent != -1){
            assert(!compare(
                values[static_cast<std::size_t>(index)],
                values[static_cast<std::size_t>(parent)]
            ));
            assert(
                tree.left[static_cast<std::size_t>(parent)] == index
                || tree.right[static_cast<std::size_t>(parent)] == index
            );
        }
    }
}

void self_test(){
    const std::vector<int> values = {3, 1, 1, 2};
    const auto minimum = cartesian_tree(values);
    validate(values, minimum, std::less<int>());
    assert(minimum.root == 1);
    assert((minimum.parent == std::vector<int>{1, -1, 1, 2}));
    assert((minimum.left == std::vector<int>{-1, 0, -1, -1}));
    assert((minimum.right == std::vector<int>{-1, 2, 3, -1}));

    const auto maximum = build_cartesian_tree(
        values,
        std::greater<int>()
    );
    validate(values, maximum, std::greater<int>());
    assert(maximum.root == 0);

    const auto empty = cartesian_tree(std::vector<int>{});
    assert(empty.empty());
    assert(empty.root == -1);
}

void print_vector(const std::vector<int>& values){
    std::cout << values.size();
    for(int value : values) std::cout << ' ' << value;
    std::cout << '\n';
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }

    while(query_count--){
        std::string mode;
        int n;
        std::cin >> mode >> n;
        std::vector<long long> values(static_cast<std::size_t>(n));
        for(long long& value : values) std::cin >> value;

        CartesianTreeResult result;
        if(mode == "MIN"){
            result = cartesian_tree(values);
            validate(values, result, std::less<long long>());
        }else{
            result = build_cartesian_tree(
                values,
                std::greater<long long>()
            );
            validate(values, result, std::greater<long long>());
        }

        std::cout
            << result.root << ' '
            << result.size() << ' '
            << result.empty() << '\n';
        print_vector(result.parent);
        print_vector(result.left);
        print_vector(result.right);
    }
}
