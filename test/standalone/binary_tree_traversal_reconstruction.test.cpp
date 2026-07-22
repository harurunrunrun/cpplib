// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/construction/binary_tree_traversal_reconstruction.hpp"

namespace{

struct CustomKey{
    int value;
};

struct CustomHash{
    std::size_t operator()(const CustomKey& key) const noexcept{
        return static_cast<std::size_t>(key.value * 17 + 31);
    }
};

struct CustomEqual{
    bool operator()(const CustomKey& left, const CustomKey& right) const noexcept{
        return left.value == right.value;
    }
};

void test_reconstruction(){
    const std::vector<int> preorder = {1, 2, 4, 5, 3, 6};
    const std::vector<int> inorder = {4, 2, 5, 1, 3, 6};
    const std::vector<int> postorder = {4, 5, 2, 6, 3, 1};
    const auto result = reconstruct_binary_tree_traversals<6>(preorder, inorder, postorder);

    assert(result.valid);
    assert(static_cast<bool>(result));
    assert(result.size() == 6);
    assert(!result.empty());
    assert(result.root == 3);
    assert(result.left_child[3] == 1);
    assert(result.right_child[3] == 4);
    assert(result.left_child[1] == 0);
    assert(result.right_child[1] == 2);
    assert(result.left_child[4] == -1);
    assert(result.right_child[4] == 5);
    assert(binary_tree_traversals_compatible<6>(preorder, inorder, postorder));
}

void test_result_copy_and_move(){
    const auto original = reconstruct_binary_tree_traversals<6>(
        std::vector<int>{1, 2, 4, 5, 3, 6},
        std::vector<int>{4, 2, 5, 1, 3, 6},
        std::vector<int>{4, 5, 2, 6, 3, 1}
    );

    BinaryTreeTraversalResult<6> copied(original);
    assert(copied.valid && copied.root == 3);
    assert(copied.left_child[3] == 1 && copied.right_child[3] == 4);

    BinaryTreeTraversalResult<6> copy_assigned;
    copy_assigned = original;
    assert(copy_assigned.valid && copy_assigned.right_child[4] == 5);

    BinaryTreeTraversalResult<6> moved(std::move(copied));
    assert(moved.valid && moved.left_child[1] == 0);
    assert(copied.valid && copied.left_child[1] == 0);

    BinaryTreeTraversalResult<6> move_assigned;
    move_assigned = std::move(copy_assigned);
    assert(move_assigned.valid && move_assigned.right_child[1] == 2);
    assert(copy_assigned.valid && copy_assigned.right_child[1] == 2);

    const BinaryTreeTraversalResult<6> invalid;
    const BinaryTreeTraversalResult<6> invalid_copy(invalid);
    assert(!invalid_copy.valid && invalid_copy.empty());
}

void test_invalid_inputs(){
    assert(!binary_tree_traversals_compatible<6>(
        std::vector<int>{1, 2, 4, 5, 3, 6},
        std::vector<int>{4, 2, 5, 1, 6, 3},
        std::vector<int>{4, 5, 2, 6, 1, 3}
    ));
    assert(!binary_tree_traversals_compatible<3>(
        std::vector<int>{1, 1, 2},
        std::vector<int>{1, 2, 3},
        std::vector<int>{2, 3, 1}
    ));
    assert(!binary_tree_traversals_compatible<3>(
        std::vector<int>{1, 2, 3},
        std::vector<int>{1, 2, 3},
        std::vector<int>{1, 2, 4}
    ));
    assert(!binary_tree_traversals_compatible<3>(
        std::vector<int>{1, 2},
        std::vector<int>{1, 2, 3},
        std::vector<int>{2, 1}
    ));
}

void test_empty_and_strings(){
    const auto empty = reconstruct_binary_tree_traversals<0>(
        std::vector<int>{},
        std::vector<int>{},
        std::vector<int>{}
    );
    assert(empty.valid);
    assert(empty.empty());
    assert(empty.root == -1);

    const std::vector<std::string> preorder = {"root", "left", "right"};
    const std::vector<std::string> inorder = {"left", "root", "right"};
    const std::vector<std::string> postorder = {"left", "right", "root"};
    assert(binary_tree_traversals_compatible<3>(preorder, inorder, postorder));

    const std::vector<CustomKey> custom_preorder = {{2}, {1}, {3}};
    const std::vector<CustomKey> custom_inorder = {{1}, {2}, {3}};
    const std::vector<CustomKey> custom_postorder = {{1}, {3}, {2}};
    assert(binary_tree_traversals_compatible<3>(
        custom_preorder,
        custom_inorder,
        custom_postorder,
        CustomHash{},
        CustomEqual{}
    ));
}

void test_iterative_chain(){
    constexpr int size = 100000;
    std::vector<int> preorder(size), inorder(size), postorder(size);
    for(int index = 0; index < size; ++index){
        preorder[static_cast<std::size_t>(index)] = index;
        inorder[static_cast<std::size_t>(index)] = index;
        postorder[static_cast<std::size_t>(index)] = size - index - 1;
    }
    const auto result = reconstruct_binary_tree_traversals<size>(preorder, inorder, postorder);
    assert(result.valid);
    assert(result.root == 0);
    for(int index = 0; index < size; ++index){
        assert(result.left_child[static_cast<std::size_t>(index)] == -1);
        assert(
            result.right_child[static_cast<std::size_t>(index)]
            == (index + 1 == size ? -1 : index + 1)
        );
    }
}

void test_capacity_exception(){
    bool thrown = false;
    try{
        (void)reconstruct_binary_tree_traversals<2>(
            std::vector<int>{1, 2, 3},
            std::vector<int>{1, 2, 3},
            std::vector<int>{3, 2, 1}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    test_reconstruction();
    test_result_copy_and_move();
    test_invalid_inputs();
    test_empty_and_strings();
    test_iterative_chain();
    test_capacity_exception();
}

}

int main(){
    int test_count;
    if(!(std::cin >> test_count)){
        self_test();
        return 0;
    }

    constexpr int max_size = 200000;
    while(test_count-- != 0){
        int preorder_size, inorder_size, postorder_size;
        std::cin >> preorder_size >> inorder_size >> postorder_size;
        std::vector<long long> preorder(static_cast<std::size_t>(preorder_size));
        std::vector<long long> inorder(static_cast<std::size_t>(inorder_size));
        std::vector<long long> postorder(static_cast<std::size_t>(postorder_size));
        for(long long& value: preorder) std::cin >> value;
        for(long long& value: inorder) std::cin >> value;
        for(long long& value: postorder) std::cin >> value;

        const auto result = reconstruct_binary_tree_traversals<max_size>(
            preorder,
            inorder,
            postorder
        );
        if(!result.valid){
            std::cout << "no\n";
            continue;
        }
        std::cout << "yes " << result.size() << ' ' << result.root;
        for(int vertex = 0; vertex < result.size(); ++vertex){
            std::cout << ' ' << result.left_child[static_cast<std::size_t>(vertex)]
                      << ' ' << result.right_child[static_cast<std::size_t>(vertex)];
        }
        std::cout << '\n';
    }
}
