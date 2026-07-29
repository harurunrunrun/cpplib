// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/search/adversarial_search.hpp"
#include "../../src/approximate/search/advanced_adversarial_search.hpp"

namespace aas = advanced_adversarial_search;

struct Tree {
    std::vector<std::vector<int>> edges;
    std::vector<int> value;
};

void verify_principal_variation(const Tree& tree, const std::vector<int>& line) {
    assert(!line.empty());
    for(std::size_t index = 1; index < line.size(); ++index) {
        const auto& adjacent = tree.edges[static_cast<std::size_t>(line[index - 1])];
        if(line[index] == line[index - 1]) continue; // a reported null-move bound
        assert(std::find(adjacent.begin(), adjacent.end(), line[index]) != adjacent.end());
    }
}

Tree fixed_tree() {
    Tree tree;
    tree.edges.resize(31);
    tree.value.assign(31, 0);
    for(int vertex = 0; vertex < 15; ++vertex) {
        tree.edges[static_cast<std::size_t>(vertex)] = {
            vertex * 2 + 1, vertex * 2 + 2
        };
    }
    tree.value = {
        3, 3, 2, 3, 4, 2, 6, 3, 5, 4, 8, 2, 7, 6, 9,
        3, 4, 5, 6, 4, 7, 8, 9, 2, 3, 7, 8, 6, 7, 9, 10
    };
    return tree;
}

void deterministic_two_player_tests() {
    const Tree tree = fixed_tree();
    const auto evaluate = [&](int state) {
        return tree.value[static_cast<std::size_t>(state)];
    };
    const auto children = [&](int state) {
        return tree.edges[static_cast<std::size_t>(state)];
    };
    const auto noisy = [&](int state) {
        auto result = children(state);
        if(result.size() > 1) result.resize(1);
        return result;
    };
    const auto key = [](int parent, int child) { return child - parent * 2; };
    const int exact = minimax_search(0, 4, evaluate, children).score;
    assert(exact == 7);

    const auto mtd = aas::mtdf_search(0, 4, 0, evaluate, children);
    const auto aspiration = aas::aspiration_search(0, 4, 0, 1, evaluate, children);
    const auto iterative = aas::iterative_deepening_search(0, 4, evaluate, children);
    assert(mtd.score == exact && aspiration.score == exact && iterative.score == exact);
    assert(mtd.statistics.re_searches > 1);
    assert(aspiration.statistics.re_searches > 0);
    assert(iterative.completed_depth == 4);

    const auto quiet = aas::quiescence_search(0, 4, evaluate, noisy);
    assert(!quiet.principal_variation.empty());

    const auto null_move = aas::null_move_pruning_search(
        0, 4, evaluate, children, [](int state) { return state; },
        aas::NullMoveOptions{1, 2}
    );
    assert(null_move.score == exact);

    const auto lmr = aas::late_move_reduction_search(
        0, 4, evaluate, children, aas::LateMoveReductionOptions{1, 1, 2}
    );
    assert(lmr.score == exact);
    assert(lmr.statistics.reduced_searches > 0);

    const auto futility_safe = aas::futility_pruning_search(
        0, 4, evaluate, children, aas::FutilityOptions<int>{100, 1}
    );
    const auto razor_safe = aas::razoring_search(
        0, 4, evaluate, children, noisy, aas::RazoringOptions<int>{100, 1, 2}
    );
    const auto probcut_safe = aas::probcut_search(
        0, 4, evaluate, children, aas::ProbCutOptions<int>{100, 1, 3}
    );
    const auto multi_safe = aas::multi_probcut_search(
        0, 4, evaluate, children,
        aas::MultiProbCutOptions<int>{{{100, 1, 3}, {200, 2, 4}}}
    );
    assert(futility_safe.score == exact && razor_safe.score == exact);
    assert(probcut_safe.score == exact && multi_safe.score == exact);

    aas::KillerHeuristic<int> killers;
    aas::HistoryHeuristic<int> history;
    aas::CountermoveHeuristic<int> countermoves;
    const auto killer_result = aas::killer_heuristic_search(
        0, 4, evaluate, children, key, killers
    );
    const auto history_result = aas::history_heuristic_search(
        0, 4, evaluate, children, key, history
    );
    const auto countermove_result = aas::countermove_heuristic_search(
        0, 4, evaluate, children, key, countermoves
    );
    assert(killer_result.score == exact);
    assert(history_result.score == exact);
    assert(countermove_result.score == exact);
    assert(killer_result.statistics.cutoffs > 0);
    assert(history_result.statistics.cutoffs > 0);
    assert(countermove_result.statistics.cutoffs > 0);

    const auto beam_exact = aas::beam_minimax_search(0, 4, 2, evaluate, children);
    const auto beam_pruned = aas::beam_minimax_search(0, 4, 1, evaluate, children);
    assert(beam_exact.score == exact);
    assert(beam_pruned.statistics.heuristic_prunes > 0);
    verify_principal_variation(tree, mtd.principal_variation);
    verify_principal_variation(tree, aspiration.principal_variation);

    bool threw = false;
    try {
        static_cast<void>(aas::aspiration_search(0, 4, 0, 0, evaluate, children));
    } catch(const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    threw = false;
    try {
        static_cast<void>(aas::beam_minimax_search(0, 4, 0, evaluate, children));
    } catch(const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

void expectimax_and_multiplayer_tests() {
    const std::vector<std::vector<int>> edges{{1, 2}, {3, 4}, {5, 6}, {}, {}, {}, {}};
    const std::vector<double> values{0, 0, 0, 2, 6, 5, 8};
    const auto children = [&](int state) { return edges[static_cast<std::size_t>(state)]; };
    const auto evaluate = [&](int state) { return values[static_cast<std::size_t>(state)]; };
    const auto node_type = [](int state) {
        return state == 0 ? aas::ExpectimaxNode::maximum : aas::ExpectimaxNode::chance;
    };
    const auto probability = [](int, int) { return 0.5; };
    const auto expected = aas::expectimax_search(
        0, 2, evaluate, children, node_type, probability
    );
    assert(std::abs(expected.score - 6.5) < 1e-12);
    assert(expected.principal_variation.front() == 0);

    struct Position {
        int node;
        std::size_t player;
    };
    const std::vector<std::vector<int>> multi_edges{{1, 2}, {3, 4}, {5, 6}, {}, {}, {}, {}};
    const std::vector<std::vector<int>> payoffs{
        {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
        {5, 1, 0}, {3, 9, 0}, {4, 2, 7}, {6, 0, 1}
    };
    const auto multi_children = [&](const Position& state) {
        std::vector<Position> result;
        for(const int next : multi_edges[static_cast<std::size_t>(state.node)])
            result.push_back(Position{next, (state.player + 1) % 3});
        return result;
    };
    const auto vector_evaluate = [&](const Position& state) {
        return payoffs[static_cast<std::size_t>(state.node)];
    };
    const auto player = [](const Position& state) { return state.player; };
    const auto maxn = aas::maxn_search(
        Position{0, 0}, 2, 3, vector_evaluate, multi_children, player
    );
    assert(maxn.scores == std::vector<int>({4, 2, 7}));
    const auto root_evaluate = [&](const Position& state) {
        return payoffs[static_cast<std::size_t>(state.node)][0];
    };
    const auto paranoid = aas::paranoid_search(
        Position{0, 0}, 2, 0, root_evaluate, multi_children, player
    );
    assert(paranoid.score == 4);
}

void monte_carlo_tests() {
    const std::vector<int> rewards{-10, 7, 30, 2};
    const auto children = [&](int state) {
        if(state == -1) return std::vector<int>{0, 1, 2, 3};
        return std::vector<int>{};
    };
    const auto evaluate = [&](int state) {
        return state < 0 ? 0 : rewards[static_cast<std::size_t>(state)];
    };
    std::mt19937 engine(20260729U);
    const auto flat = aas::flat_monte_carlo_search(
        -1, 200, 1, evaluate, children, engine
    );
    assert(flat.principal_variation.size() >= 2 && flat.principal_variation[1] == 2);
    assert(flat.statistics.simulations == 200);
    std::mt19937 tree_engine(998244353U);
    const auto tree = aas::monte_carlo_search(
        -1, 2000, 1, evaluate, children, tree_engine, 0.5
    );
    assert(tree.principal_variation.size() >= 2 && tree.principal_variation[1] == 2);
    assert(tree.statistics.simulations == 2000);
}

int main() {
    deterministic_two_player_tests();
    expectimax_and_multiplayer_tests();
    monte_carlo_tests();

    int leaf_count;
    if(!(std::cin >> leaf_count)) return 0;
    std::vector<int> values(static_cast<std::size_t>(leaf_count));
    for(int& value : values) std::cin >> value;
    const int base = leaf_count - 1;
    std::vector<std::vector<int>> edges(static_cast<std::size_t>(2 * leaf_count - 1));
    for(int vertex = 0; vertex < base; ++vertex) {
        edges[static_cast<std::size_t>(vertex)] = {vertex * 2 + 1, vertex * 2 + 2};
    }
    const auto children = [&](int state) { return edges[static_cast<std::size_t>(state)]; };
    const auto evaluate = [&](int state) {
        return state < base ? 0 : values[static_cast<std::size_t>(state - base)];
    };
    std::size_t depth = 0;
    for(int count = leaf_count; 1 < count; count /= 2) ++depth;
    const auto result = aas::mtdf_search(0, depth, 0, evaluate, children);
    std::cout << result.score << '\n';
}
