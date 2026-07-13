// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include "../../src/approximate/search/adversarial_search.hpp"

struct CompleteTree{
    int depth;
    std::vector<int> leaves;

    std::vector<int> children(int vertex) const{
        const int first_leaf = (1 << depth) - 1;
        if(first_leaf <= vertex) return {};
        return {2 * vertex + 1, 2 * vertex + 2};
    }

    int root_evaluation(int vertex) const{
        const int first_leaf = (1 << depth) - 1;
        if(vertex < first_leaf) return 0;
        return leaves[static_cast<std::size_t>(vertex - first_leaf)];
    }

    int side_to_move_evaluation(int vertex) const{
        const int value = root_evaluation(vertex);
        return depth % 2 == 0 ? value : -value;
    }
};

void check_tree(const CompleteTree& tree){
    const auto children = [&](int vertex){ return tree.children(vertex); };
    const auto fixed = [&](int vertex){ return tree.root_evaluation(vertex); };
    const auto relative = [&](int vertex){ return tree.side_to_move_evaluation(vertex); };
    const auto minimax = minimax_search(0, static_cast<std::size_t>(tree.depth), fixed, children);
    const auto alpha_beta = alpha_beta_search(
        0, static_cast<std::size_t>(tree.depth), fixed, children
    );
    const auto negamax = negamax_search(
        0, static_cast<std::size_t>(tree.depth), relative, children
    );
    const auto pvs = principal_variation_search(
        0, static_cast<std::size_t>(tree.depth), relative, children
    );
    const auto negascout = negascout_search(
        0, static_cast<std::size_t>(tree.depth), relative, children
    );
    assert(minimax.score == alpha_beta.score);
    assert(minimax.score == negamax.score);
    assert(minimax.score == pvs.score);
    assert(minimax.score == negascout.score);
    assert(minimax.principal_variation.size() == static_cast<std::size_t>(tree.depth + 1));
    assert(alpha_beta.visited_nodes <= minimax.visited_nodes);
}

void self_test(){
    std::mt19937 random_engine(17012026U);
    for(int depth = 0; depth <= 8; ++depth){
        for(int repetition = 0; repetition < 100; ++repetition){
            CompleteTree tree{depth, std::vector<int>(static_cast<std::size_t>(1 << depth))};
            for(int& value : tree.leaves){
                value = static_cast<int>(random_engine() % 2001U) - 1000;
            }
            check_tree(tree);
        }
    }
}

int main(){
    int depth;
    if(!(std::cin >> depth)){
        self_test();
        return 0;
    }
    CompleteTree tree{depth, std::vector<int>(static_cast<std::size_t>(1 << depth))};
    for(int& value : tree.leaves) std::cin >> value;
    const auto children = [&](int vertex){ return tree.children(vertex); };
    const auto fixed = [&](int vertex){ return tree.root_evaluation(vertex); };
    const auto relative = [&](int vertex){ return tree.side_to_move_evaluation(vertex); };
    const auto minimax = minimax_search(0, static_cast<std::size_t>(depth), fixed, children);
    const auto alpha_beta = alpha_beta_search(0, static_cast<std::size_t>(depth), fixed, children);
    const auto negamax = negamax_search(0, static_cast<std::size_t>(depth), relative, children);
    const auto pvs = principal_variation_search(0, static_cast<std::size_t>(depth), relative, children);
    const auto negascout = negascout_search(0, static_cast<std::size_t>(depth), relative, children);
    std::cout << minimax.score << ' ' << alpha_beta.score << ' '
              << negamax.score << ' ' << pvs.score << ' ' << negascout.score << '\n';
}
