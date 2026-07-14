// competitive-verifier: PROBLEM https://www.spoj.com/problems/TREEORD/

#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/binary_tree_traversal_reconstruction.hpp"

int main(){
    int n;
    std::cin >> n;
    std::vector<int> preorder(static_cast<std::size_t>(n));
    std::vector<int> postorder(static_cast<std::size_t>(n));
    std::vector<int> inorder(static_cast<std::size_t>(n));
    for(int& value: preorder) std::cin >> value;
    for(int& value: postorder) std::cin >> value;
    for(int& value: inorder) std::cin >> value;

    std::cout << (
        binary_tree_traversals_compatible<8000>(preorder, inorder, postorder)
        ? "yes\n"
        : "no\n"
    );
}
