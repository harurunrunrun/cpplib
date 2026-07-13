// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/search/monte_carlo_tree_search.hpp"

struct BanditState{
    int selected;
};

void self_test(){
    const std::vector<int> rewards{-8, 3, 20, 7};
    const auto terminal = [](const BanditState& state){ return state.selected != -1; };
    const auto moves = [&](const BanditState& state){
        std::vector<int> result;
        if(state.selected == -1){
            for(std::size_t index = 0; index < rewards.size(); ++index){
                result.push_back(static_cast<int>(index));
            }
        }
        return result;
    };
    const auto transition = [](const BanditState&, int move){ return BanditState{move}; };
    const auto player = [](const BanditState& state){ return state.selected == -1 ? 0 : 1; };
    const auto reward = [&](const BanditState& state, int){
        return static_cast<double>(rewards[static_cast<std::size_t>(state.selected)]);
    };
    std::mt19937 random_engine(123456789U);
    const auto result = monte_carlo_tree_search(
        BanditState{-1}, 2000, terminal, moves, transition,
        player, reward, random_engine, 1.0, 1
    );
    assert(result.move && *result.move == 2);
    assert(result.tree_nodes == rewards.size() + 1);

    struct GameState{
        int depth;
        int root_move;
        int reply;
    };
    const auto game_terminal = [](const GameState& state){ return state.depth == 2; };
    const auto game_moves = [](const GameState& state){
        return state.depth < 2 ? std::vector<int>{0, 1} : std::vector<int>{};
    };
    const auto game_transition = [](const GameState& state, int move){
        if(state.depth == 0) return GameState{1, move, -1};
        return GameState{2, state.root_move, move};
    };
    const auto game_player = [](const GameState& state){ return state.depth % 2; };
    const auto game_reward = [](const GameState& state, int){
        constexpr double payoff[2][2] = {{1.0, -1.0}, {0.2, 0.0}};
        return payoff[state.root_move][state.reply];
    };
    std::mt19937 game_engine(42424242U);
    const auto game_result = monte_carlo_tree_search(
        GameState{0, -1, -1}, 10000,
        game_terminal, game_moves, game_transition,
        game_player, game_reward, game_engine, 1.0, 2
    );
    assert(game_result.move && *game_result.move == 1);

    bool threw = false;
    try{
        static_cast<void>(monte_carlo_tree_search(
            BanditState{-1}, 1, terminal, moves, transition,
            player, reward, random_engine, std::numeric_limits<double>::infinity(), 1
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

int main(){
    int count;
    std::size_t iterations;
    if(!(std::cin >> count >> iterations)){
        self_test();
        return 0;
    }
    std::vector<int> rewards(static_cast<std::size_t>(count));
    for(int& value : rewards) std::cin >> value;
    const auto terminal = [](const BanditState& state){ return state.selected != -1; };
    const auto moves = [count](const BanditState& state){
        std::vector<int> result;
        if(state.selected == -1){
            for(int move = 0; move < count; ++move) result.push_back(move);
        }
        return result;
    };
    const auto transition = [](const BanditState&, int move){ return BanditState{move}; };
    const auto player = [](const BanditState& state){ return state.selected == -1 ? 0 : 1; };
    const auto reward = [&](const BanditState& state, int){
        return static_cast<double>(rewards[static_cast<std::size_t>(state.selected)]);
    };
    std::mt19937 random_engine(998244353U);
    const auto result = monte_carlo_tree_search(
        BanditState{-1}, iterations, terminal, moves, transition,
        player, reward, random_engine, 0.7, 1
    );
    std::cout << result.move.value_or(-1) << '\n';
}
