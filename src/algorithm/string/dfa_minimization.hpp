#ifndef CPPLIB_SRC_ALGORITHM_STRING_DFA_MINIMIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_DFA_MINIMIZATION_HPP_INCLUDED

#include <cstddef>
#include <deque>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct DfaEquivalenceClasses{
    std::vector<int> state_to_class;
    std::vector<std::vector<int>> classes;
};

struct DfaMinimizationResult{
    std::vector<std::vector<int>> transition;
    std::vector<char> accepting;
    int initial_state = -1;
    std::vector<int> original_to_minimized;
    std::vector<std::vector<int>> minimized_to_original;
};

namespace dfa_minimization_internal{

struct Partition{
    std::vector<int> state_to_block;
    std::vector<std::vector<int>> blocks;
};

inline int validate_complete_dfa(
    const std::vector<std::vector<int>>& transition,
    const std::vector<char>& accepting
){
    if(transition.size() != accepting.size() ||
        transition.size() >
            static_cast<std::size_t>(std::numeric_limits<int>::max())
    )[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid DFA (Hopcroft minimization)."
        );
    }

    const std::size_t alphabet_size =
        transition.empty() ? 0 : transition.front().size();
    if(alphabet_size >
        static_cast<std::size_t>(std::numeric_limits<int>::max())
    )[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid DFA (Hopcroft minimization)."
        );
    }

    const int state_count = static_cast<int>(transition.size());
    for(const auto& row: transition){
        if(row.size() != alphabet_size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid DFA "
                "(Hopcroft minimization)."
            );
        }
        for(int to: row){
            if(to < 0 || state_count <= to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid DFA "
                    "(Hopcroft minimization)."
                );
            }
        }
    }
    return static_cast<int>(alphabet_size);
}

inline Partition hopcroft_partition(
    const std::vector<std::vector<int>>& transition,
    const std::vector<char>& accepting,
    const std::vector<char>& active,
    int alphabet_size
){
    const int state_count = static_cast<int>(transition.size());
    Partition result;
    result.state_to_block.assign(
        static_cast<std::size_t>(state_count),
        -1
    );

    std::vector<int> rejecting;
    std::vector<int> accepting_states;
    for(int state = 0; state < state_count; state++){
        if(!active[static_cast<std::size_t>(state)]) continue;
        if(accepting[static_cast<std::size_t>(state)]){
            accepting_states.push_back(state);
        }else{
            rejecting.push_back(state);
        }
    }
    if(!rejecting.empty()) result.blocks.push_back(std::move(rejecting));
    if(!accepting_states.empty()){
        result.blocks.push_back(std::move(accepting_states));
    }
    if(result.blocks.empty()) return result;

    int active_count = 0;
    std::vector<int> active_index(
        static_cast<std::size_t>(state_count),
        -1
    );
    for(int state = 0; state < state_count; state++){
        if(active[static_cast<std::size_t>(state)]){
            active_index[static_cast<std::size_t>(state)] = active_count++;
        }
    }
    result.blocks.reserve(static_cast<std::size_t>(active_count));

    std::vector<int> position_in_block(
        static_cast<std::size_t>(state_count),
        -1
    );
    for(int block = 0; block < static_cast<int>(result.blocks.size()); block++){
        for(int position = 0;
            position < static_cast<int>(
                result.blocks[static_cast<std::size_t>(block)].size()
            );
            position++
        ){
            const int state =
                result.blocks[static_cast<std::size_t>(block)]
                             [static_cast<std::size_t>(position)];
            result.state_to_block[static_cast<std::size_t>(state)] = block;
            position_in_block[static_cast<std::size_t>(state)] = position;
        }
    }

    const std::size_t inverse_size =
        static_cast<std::size_t>(alphabet_size) *
        static_cast<std::size_t>(active_count);
    std::vector<std::vector<int>> inverse(inverse_size);
    for(int state = 0; state < state_count; state++){
        if(!active[static_cast<std::size_t>(state)]) continue;
        for(int symbol = 0; symbol < alphabet_size; symbol++){
            const int to = transition[static_cast<std::size_t>(state)]
                                     [static_cast<std::size_t>(symbol)];
            inverse[
                static_cast<std::size_t>(symbol) *
                    static_cast<std::size_t>(active_count) +
                static_cast<std::size_t>(
                    active_index[static_cast<std::size_t>(to)]
                )
            ].push_back(state);
        }
    }

    std::deque<std::pair<int, int>> work;
    std::vector<std::vector<char>> queued(
        result.blocks.size(),
        std::vector<char>(static_cast<std::size_t>(alphabet_size))
    );
    queued.reserve(static_cast<std::size_t>(active_count));
    const auto enqueue = [&](int block, int symbol){
        char& flag = queued[static_cast<std::size_t>(block)]
                           [static_cast<std::size_t>(symbol)];
        if(flag) return;
        flag = 1;
        work.emplace_back(block, symbol);
    };

    int initial_splitter = 0;
    if(result.blocks.size() == 2 &&
        result.blocks[1].size() < result.blocks[0].size()
    ){
        initial_splitter = 1;
    }
    for(int symbol = 0; symbol < alphabet_size; symbol++){
        enqueue(initial_splitter, symbol);
    }

    std::vector<char> in_preimage(
        static_cast<std::size_t>(state_count)
    );
    std::vector<std::vector<int>> intersections(result.blocks.size());
    intersections.reserve(static_cast<std::size_t>(active_count));

    while(!work.empty()){
        const auto [splitter, symbol] = work.front();
        work.pop_front();
        queued[static_cast<std::size_t>(splitter)]
              [static_cast<std::size_t>(symbol)] = 0;

        std::vector<int> preimage;
        std::vector<int> affected_blocks;
        for(int state: result.blocks[static_cast<std::size_t>(splitter)]){
            const auto& predecessors = inverse[
                static_cast<std::size_t>(symbol) *
                    static_cast<std::size_t>(active_count) +
                static_cast<std::size_t>(
                    active_index[static_cast<std::size_t>(state)]
                )
            ];
            for(int predecessor: predecessors){
                char& marked =
                    in_preimage[static_cast<std::size_t>(predecessor)];
                if(marked) continue;
                marked = 1;
                preimage.push_back(predecessor);
                const int block = result.state_to_block[
                    static_cast<std::size_t>(predecessor)
                ];
                if(intersections[static_cast<std::size_t>(block)].empty()){
                    affected_blocks.push_back(block);
                }
                intersections[static_cast<std::size_t>(block)].push_back(
                    predecessor
                );
            }
        }

        for(int block: affected_blocks){
            auto& intersection =
                intersections[static_cast<std::size_t>(block)];
            auto& old_block =
                result.blocks[static_cast<std::size_t>(block)];
            if(intersection.size() == old_block.size()){
                intersection.clear();
                continue;
            }

            const int new_block =
                static_cast<int>(result.blocks.size());
            result.blocks.emplace_back();
            result.blocks.back().reserve(intersection.size());
            queued.emplace_back(
                static_cast<std::size_t>(alphabet_size),
                0
            );
            intersections.emplace_back();

            for(int state: intersection){
                const int position =
                    position_in_block[static_cast<std::size_t>(state)];
                const int moved = old_block.back();
                old_block[static_cast<std::size_t>(position)] = moved;
                position_in_block[static_cast<std::size_t>(moved)] = position;
                old_block.pop_back();

                position_in_block[static_cast<std::size_t>(state)] =
                    static_cast<int>(result.blocks.back().size());
                result.blocks.back().push_back(state);
                result.state_to_block[static_cast<std::size_t>(state)] =
                    new_block;
            }

            for(int next_symbol = 0;
                next_symbol < alphabet_size;
                next_symbol++
            ){
                if(queued[static_cast<std::size_t>(block)]
                         [static_cast<std::size_t>(next_symbol)]
                ){
                    enqueue(new_block, next_symbol);
                }else if(old_block.size() <=
                    result.blocks[static_cast<std::size_t>(new_block)].size()
                ){
                    enqueue(block, next_symbol);
                }else{
                    enqueue(new_block, next_symbol);
                }
            }
            intersection.clear();
        }

        for(int state: preimage){
            in_preimage[static_cast<std::size_t>(state)] = 0;
        }
    }

    return result;
}

} // namespace dfa_minimization_internal

inline DfaEquivalenceClasses hopcroft_dfa_equivalence_classes(
    const std::vector<std::vector<int>>& transition,
    const std::vector<char>& accepting
){
    const int alphabet_size =
        dfa_minimization_internal::validate_complete_dfa(
            transition,
            accepting
        );
    const int state_count = static_cast<int>(transition.size());
    const std::vector<char> active(
        static_cast<std::size_t>(state_count),
        1
    );
    auto partition = dfa_minimization_internal::hopcroft_partition(
        transition,
        accepting,
        active,
        alphabet_size
    );

    DfaEquivalenceClasses result;
    result.state_to_class.assign(
        static_cast<std::size_t>(state_count),
        -1
    );
    std::vector<int> block_to_class(partition.blocks.size(), -1);
    for(int state = 0; state < state_count; state++){
        const int block =
            partition.state_to_block[static_cast<std::size_t>(state)];
        int& class_id =
            block_to_class[static_cast<std::size_t>(block)];
        if(class_id == -1){
            class_id = static_cast<int>(result.classes.size());
            result.classes.emplace_back();
        }
        result.state_to_class[static_cast<std::size_t>(state)] = class_id;
        result.classes[static_cast<std::size_t>(class_id)].push_back(state);
    }
    return result;
}

inline DfaMinimizationResult hopcroft_minimize_dfa(
    const std::vector<std::vector<int>>& transition,
    const std::vector<char>& accepting,
    int initial_state
){
    const int alphabet_size =
        dfa_minimization_internal::validate_complete_dfa(
            transition,
            accepting
        );
    const int state_count = static_cast<int>(transition.size());
    if(initial_state < 0 || state_count <= initial_state)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid initial state "
            "(Hopcroft minimization)."
        );
    }

    std::vector<char> active(
        static_cast<std::size_t>(state_count)
    );
    std::vector<int> stack{initial_state};
    active[static_cast<std::size_t>(initial_state)] = 1;
    while(!stack.empty()){
        const int state = stack.back();
        stack.pop_back();
        for(int to: transition[static_cast<std::size_t>(state)]){
            char& reached = active[static_cast<std::size_t>(to)];
            if(reached) continue;
            reached = 1;
            stack.push_back(to);
        }
    }

    auto partition = dfa_minimization_internal::hopcroft_partition(
        transition,
        accepting,
        active,
        alphabet_size
    );
    std::vector<int> block_to_minimized(
        partition.blocks.size(),
        -1
    );
    std::vector<int> order;
    const int initial_block =
        partition.state_to_block[static_cast<std::size_t>(initial_state)];
    block_to_minimized[static_cast<std::size_t>(initial_block)] = 0;
    order.push_back(initial_block);

    DfaMinimizationResult result;
    result.initial_state = 0;
    for(std::size_t index = 0; index < order.size(); index++){
        const int block = order[index];
        const int representative =
            partition.blocks[static_cast<std::size_t>(block)].front();
        std::vector<int> row(
            static_cast<std::size_t>(alphabet_size)
        );
        for(int symbol = 0; symbol < alphabet_size; symbol++){
            const int to =
                transition[static_cast<std::size_t>(representative)]
                          [static_cast<std::size_t>(symbol)];
            const int to_block =
                partition.state_to_block[static_cast<std::size_t>(to)];
            int& minimized =
                block_to_minimized[static_cast<std::size_t>(to_block)];
            if(minimized == -1){
                minimized = static_cast<int>(order.size());
                order.push_back(to_block);
            }
            row[static_cast<std::size_t>(symbol)] = minimized;
        }
        result.transition.push_back(std::move(row));
        result.accepting.push_back(
            accepting[static_cast<std::size_t>(representative)] != 0
        );
    }

    result.original_to_minimized.assign(
        static_cast<std::size_t>(state_count),
        -1
    );
    result.minimized_to_original.resize(order.size());
    for(int state = 0; state < state_count; state++){
        if(!active[static_cast<std::size_t>(state)]) continue;
        const int block =
            partition.state_to_block[static_cast<std::size_t>(state)];
        const int minimized =
            block_to_minimized[static_cast<std::size_t>(block)];
        result.original_to_minimized[static_cast<std::size_t>(state)] =
            minimized;
        result.minimized_to_original[
            static_cast<std::size_t>(minimized)
        ].push_back(state);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_DFA_MINIMIZATION_HPP_INCLUDED
