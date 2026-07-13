// competitive-verifier: STANDALONE

#include "../../src/algorithm/string/dfa_minimization.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <vector>

std::vector<int> naive_equivalence_classes(
    const std::vector<std::vector<int>>& transition,
    const std::vector<char>& accepting,
    const std::vector<char>& active
){
    const int n = static_cast<int>(transition.size());
    std::vector<int> classes(static_cast<std::size_t>(n), -1);
    for(int state = 0; state < n; state++){
        if(active[static_cast<std::size_t>(state)]){
            classes[static_cast<std::size_t>(state)] =
                accepting[static_cast<std::size_t>(state)] != 0;
        }
    }

    std::vector<int> next(static_cast<std::size_t>(n), -1);
    while(true){
        std::map<std::vector<int>, int> ids;
        for(int state = 0; state < n; state++){
            if(!active[static_cast<std::size_t>(state)]) continue;
            std::vector<int> signature;
            signature.reserve(
                transition[static_cast<std::size_t>(state)].size() + 1
            );
            signature.push_back(
                accepting[static_cast<std::size_t>(state)] != 0
            );
            for(int to: transition[static_cast<std::size_t>(state)]){
                signature.push_back(classes[static_cast<std::size_t>(to)]);
            }

            auto it = ids.find(signature);
            if(it == ids.end()){
                const int id = static_cast<int>(ids.size());
                it = ids.emplace(std::move(signature), id).first;
            }
            next[static_cast<std::size_t>(state)] = it->second;
        }

        bool changed = false;
        for(int state = 0; state < n; state++){
            if(active[static_cast<std::size_t>(state)] &&
                classes[static_cast<std::size_t>(state)] !=
                    next[static_cast<std::size_t>(state)]
            ){
                changed = true;
            }
        }
        classes.swap(next);
        std::fill(next.begin(), next.end(), -1);
        if(!changed) return classes;
    }
}

DfaMinimizationResult naive_minimize_dfa(
    const std::vector<std::vector<int>>& transition,
    const std::vector<char>& accepting,
    int initial_state
){
    const int n = static_cast<int>(transition.size());
    const int alphabet_size =
        static_cast<int>(transition.front().size());
    std::vector<char> active(static_cast<std::size_t>(n));
    std::vector<int> stack{initial_state};
    active[static_cast<std::size_t>(initial_state)] = 1;
    while(!stack.empty()){
        const int state = stack.back();
        stack.pop_back();
        for(int to: transition[static_cast<std::size_t>(state)]){
            if(active[static_cast<std::size_t>(to)]) continue;
            active[static_cast<std::size_t>(to)] = 1;
            stack.push_back(to);
        }
    }

    const auto classes =
        naive_equivalence_classes(transition, accepting, active);
    int class_count = 0;
    for(int value: classes) class_count = std::max(class_count, value + 1);
    std::vector<int> representative(
        static_cast<std::size_t>(class_count),
        -1
    );
    for(int state = 0; state < n; state++){
        if(!active[static_cast<std::size_t>(state)]) continue;
        int& value = representative[
            static_cast<std::size_t>(
                classes[static_cast<std::size_t>(state)]
            )
        ];
        if(value == -1) value = state;
    }

    std::vector<int> class_to_minimized(
        static_cast<std::size_t>(class_count),
        -1
    );
    std::vector<int> order{
        classes[static_cast<std::size_t>(initial_state)]
    };
    class_to_minimized[static_cast<std::size_t>(order.front())] = 0;

    DfaMinimizationResult result;
    result.initial_state = 0;
    for(std::size_t index = 0; index < order.size(); index++){
        const int class_id = order[index];
        const int state =
            representative[static_cast<std::size_t>(class_id)];
        std::vector<int> row(
            static_cast<std::size_t>(alphabet_size)
        );
        for(int symbol = 0; symbol < alphabet_size; symbol++){
            const int to = transition[static_cast<std::size_t>(state)]
                                     [static_cast<std::size_t>(symbol)];
            const int to_class = classes[static_cast<std::size_t>(to)];
            int& minimized =
                class_to_minimized[static_cast<std::size_t>(to_class)];
            if(minimized == -1){
                minimized = static_cast<int>(order.size());
                order.push_back(to_class);
            }
            row[static_cast<std::size_t>(symbol)] = minimized;
        }
        result.transition.push_back(std::move(row));
        result.accepting.push_back(
            accepting[static_cast<std::size_t>(state)] != 0
        );
    }

    result.original_to_minimized.assign(
        static_cast<std::size_t>(n),
        -1
    );
    result.minimized_to_original.resize(order.size());
    for(int state = 0; state < n; state++){
        if(!active[static_cast<std::size_t>(state)]) continue;
        const int minimized = class_to_minimized[
            static_cast<std::size_t>(
                classes[static_cast<std::size_t>(state)]
            )
        ];
        result.original_to_minimized[static_cast<std::size_t>(state)] =
            minimized;
        result.minimized_to_original[
            static_cast<std::size_t>(minimized)
        ].push_back(state);
    }
    return result;
}

void assert_same_result(
    const DfaMinimizationResult& actual,
    const DfaMinimizationResult& expected
){
    assert(actual.transition == expected.transition);
    assert(actual.accepting == expected.accepting);
    assert(actual.initial_state == expected.initial_state);
    assert(
        actual.original_to_minimized == expected.original_to_minimized
    );
    assert(
        actual.minimized_to_original == expected.minimized_to_original
    );
}

void check_case(
    const std::vector<std::vector<int>>& transition,
    const std::vector<char>& accepting,
    int initial_state
){
    const int n = static_cast<int>(transition.size());
    assert_same_result(
        hopcroft_minimize_dfa(transition, accepting, initial_state),
        naive_minimize_dfa(transition, accepting, initial_state)
    );

    const std::vector<char> all_active(
        static_cast<std::size_t>(n),
        1
    );
    const auto naive =
        naive_equivalence_classes(transition, accepting, all_active);
    const auto actual =
        hopcroft_dfa_equivalence_classes(transition, accepting);
    assert(actual.state_to_class.size() == transition.size());

    int previous_minimum = -1;
    for(const auto& states: actual.classes){
        assert(!states.empty());
        assert(std::is_sorted(states.begin(), states.end()));
        assert(previous_minimum < states.front());
        previous_minimum = states.front();
    }
    for(int left = 0; left < n; left++){
        for(int right = 0; right < n; right++){
            assert(
                (actual.state_to_class[static_cast<std::size_t>(left)] ==
                    actual.state_to_class[static_cast<std::size_t>(right)]) ==
                (naive[static_cast<std::size_t>(left)] ==
                    naive[static_cast<std::size_t>(right)])
            );
        }
    }
}

template<class Function>
bool throws_runtime_error(Function function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

void self_test(){
    {
        const std::vector<std::vector<int>> transition;
        const std::vector<char> accepting;
        const auto result =
            hopcroft_dfa_equivalence_classes(transition, accepting);
        assert(result.state_to_class.empty());
        assert(result.classes.empty());
        assert(throws_runtime_error([&]{
            (void)hopcroft_minimize_dfa(transition, accepting, 0);
        }));
    }
    {
        const std::vector<std::vector<int>> transition(3);
        const std::vector<char> accepting{1, 0, 1};
        check_case(transition, accepting, 1);
        const auto partition =
            hopcroft_dfa_equivalence_classes(transition, accepting);
        assert((partition.classes ==
            std::vector<std::vector<int>>{{0, 2}, {1}}));
    }
    {
        const std::vector<std::vector<int>> transition{
            {1, 2},
            {1, 3},
            {1, 3},
            {3, 3},
            {5, 4},
            {4, 5},
        };
        const std::vector<char> accepting{0, 0, 0, 1, 0, 1};
        check_case(transition, accepting, 0);
        const auto result =
            hopcroft_minimize_dfa(transition, accepting, 0);
        assert(result.original_to_minimized[1] ==
            result.original_to_minimized[2]);
        assert(result.original_to_minimized[4] == -1);
        assert(result.original_to_minimized[5] == -1);
    }
    {
        const std::vector<std::vector<int>> transition{
            {1, 2, 0},
            {2, 0, 1},
            {0, 1, 2},
        };
        const std::vector<char> accepting(3);
        check_case(transition, accepting, 0);
        const auto result =
            hopcroft_minimize_dfa(transition, accepting, 0);
        assert(result.transition.size() == 1);
        assert((result.transition[0] == std::vector<int>{0, 0, 0}));
    }
    {
        const std::vector<std::vector<int>> transition{{0}};
        const std::vector<char> accepting{1};
        check_case(transition, accepting, 0);
    }
    {
        const std::vector<std::vector<int>> transition{{0}};
        assert(throws_runtime_error([&]{
            (void)hopcroft_dfa_equivalence_classes(transition, {});
        }));
        assert(throws_runtime_error([&]{
            (void)hopcroft_dfa_equivalence_classes(
                {{0}, {0, 1}},
                {0, 1}
            );
        }));
        assert(throws_runtime_error([&]{
            (void)hopcroft_dfa_equivalence_classes({{1}}, {0});
        }));
        assert(throws_runtime_error([&]{
            (void)hopcroft_minimize_dfa(transition, {0}, -1);
        }));
        assert(throws_runtime_error([&]{
            (void)hopcroft_minimize_dfa(transition, {0}, 1);
        }));
    }

    std::mt19937 rng(20260802);
    for(int iteration = 0; iteration < 2600; iteration++){
        const int n = 1 + static_cast<int>(rng() % 18U);
        const int alphabet_size = static_cast<int>(rng() % 6U);
        std::vector<std::vector<int>> transition(
            static_cast<std::size_t>(n),
            std::vector<int>(static_cast<std::size_t>(alphabet_size))
        );
        std::vector<char> accepting(static_cast<std::size_t>(n));
        for(int state = 0; state < n; state++){
            accepting[static_cast<std::size_t>(state)] =
                static_cast<char>(rng() % 2U);
            for(int symbol = 0; symbol < alphabet_size; symbol++){
                transition[static_cast<std::size_t>(state)]
                          [static_cast<std::size_t>(symbol)] =
                    static_cast<int>(
                        rng() % static_cast<unsigned>(n)
                    );
            }
        }
        const int initial_state = static_cast<int>(
            rng() % static_cast<unsigned>(n)
        );
        check_case(transition, accepting, initial_state);
    }

    {
        constexpr int n = 8000;
        std::vector<std::vector<int>> transition(
            n,
            std::vector<int>(1)
        );
        std::vector<char> accepting(n);
        for(int state = 0; state < n; state++){
            transition[static_cast<std::size_t>(state)][0] =
                std::min(state + 1, n - 1);
        }
        accepting.back() = 1;
        const auto result =
            hopcroft_minimize_dfa(transition, accepting, 0);
        assert(static_cast<int>(result.transition.size()) == n);
        assert(result.original_to_minimized.front() == 0);
        assert(result.original_to_minimized.back() == n - 1);
    }
    {
        constexpr int n = 10000;
        constexpr int alphabet_size = 3;
        std::vector<std::vector<int>> transition(
            n,
            std::vector<int>(alphabet_size)
        );
        for(int state = 0; state < n; state++){
            transition[static_cast<std::size_t>(state)][0] =
                (state + 1) % n;
            transition[static_cast<std::size_t>(state)][1] =
                (state * 17 + 3) % n;
            transition[static_cast<std::size_t>(state)][2] = state;
        }
        const std::vector<char> accepting(n);
        const auto result =
            hopcroft_minimize_dfa(transition, accepting, 137);
        assert(result.transition.size() == 1);
        assert(result.original_to_minimized ==
            std::vector<int>(static_cast<std::size_t>(n), 0));
    }
}

void print_ints(const std::vector<int>& values){
    for(std::size_t index = 0; index < values.size(); index++){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

void print_flags(const std::vector<char>& values){
    for(std::size_t index = 0; index < values.size(); index++){
        if(index != 0) std::cout << ' ';
        std::cout << static_cast<int>(values[index] != 0);
    }
    std::cout << '\n';
}

int main(){
    int n, alphabet_size, initial_state;
    if(!(std::cin >> n >> alphabet_size >> initial_state)){
        self_test();
        return 0;
    }

    std::vector<char> accepting(static_cast<std::size_t>(n));
    for(char& value: accepting){
        int input;
        std::cin >> input;
        value = input != 0;
    }
    std::vector<std::vector<int>> transition(
        static_cast<std::size_t>(n),
        std::vector<int>(static_cast<std::size_t>(alphabet_size))
    );
    for(auto& row: transition){
        for(int& to: row) std::cin >> to;
    }

    const auto result =
        hopcroft_minimize_dfa(transition, accepting, initial_state);
    std::cout << result.transition.size() << ' '
              << alphabet_size << ' '
              << result.initial_state << '\n';
    print_flags(result.accepting);
    print_ints(result.original_to_minimized);
    for(const auto& row: result.transition) print_ints(row);
}
