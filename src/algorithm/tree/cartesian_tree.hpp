#ifndef CPPLIB_SRC_ALGORITHM_TREE_CARTESIAN_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_CARTESIAN_TREE_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

struct CartesianTreeResult{
    std::vector<int> parent;
    std::vector<int> left;
    std::vector<int> right;
    int root = -1;

    int size() const{
        return static_cast<int>(parent.size());
    }

    bool empty() const{
        return parent.empty();
    }
};

template<class T, class Compare = std::less<T>>
CartesianTreeResult cartesian_tree(
    const std::vector<T>& values,
    Compare compare = Compare()
){
    const int n = static_cast<int>(values.size());
    CartesianTreeResult result{
        std::vector<int>(static_cast<std::size_t>(n), -1),
        std::vector<int>(static_cast<std::size_t>(n), -1),
        std::vector<int>(static_cast<std::size_t>(n), -1),
        -1
    };
    if(n == 0) return result;

    std::vector<int> stack;
    stack.reserve(values.size());

    for(int index = 0; index < n; ++index){
        int last = -1;
        while(
            !stack.empty()
            && compare(
                values[static_cast<std::size_t>(index)],
                values[static_cast<std::size_t>(stack.back())]
            )
        ){
            last = stack.back();
            stack.pop_back();
        }

        if(!stack.empty()){
            const int parent = stack.back();
            result.parent[static_cast<std::size_t>(index)] = parent;
            result.right[static_cast<std::size_t>(parent)] = index;
        }
        if(last != -1){
            result.parent[static_cast<std::size_t>(last)] = index;
            result.left[static_cast<std::size_t>(index)] = last;
        }
        stack.push_back(index);
    }

    result.root = stack.front();
    return result;
}

template<class T, class Compare = std::less<T>>
CartesianTreeResult build_cartesian_tree(
    const std::vector<T>& values,
    Compare compare = Compare()
){
    return cartesian_tree(values, std::move(compare));
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_CARTESIAN_TREE_HPP_INCLUDED
