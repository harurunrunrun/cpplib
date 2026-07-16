// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../src/structure/heap/partially_retroactive_priority_queue.hpp"

using Queue = PartiallyRetroactivePriorityQueue<long long, int, 4096>;

std::optional<std::vector<int>> replay_naive(
    const std::map<long long, std::optional<int>>& history
){
    std::multiset<int> values;
    for(const auto& [time, value]: history){
        (void)time;
        if(value.has_value()) values.insert(*value);
        else{
            if(values.empty()) return std::nullopt;
            values.erase(values.begin());
        }
    }
    return std::vector<int>(values.begin(), values.end());
}

template<class Function>
bool throws_runtime_error(Function&& function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

void check_queue(const Queue& queue, const std::vector<int>& expected){
    assert(queue.size() == expected.size());
    assert(queue.empty() == expected.empty());
    assert(queue.values() == expected);
    if(expected.empty()) assert(throws_runtime_error([&]{ (void)queue.top(); }));
    else assert(queue.top() == expected.front());
}

struct NoDefault{
    int value;
    NoDefault() = delete;
    explicit NoDefault(int value_): value(value_){}
};

struct NoDefaultLess{
    bool operator()(const NoDefault& left, const NoDefault& right) const{
        return left.value < right.value;
    }
};

struct NoDefaultTime{
    int value;
    NoDefaultTime() = delete;
    explicit NoDefaultTime(int value_): value(value_){}
};

struct NoDefaultTimeLess{
    bool operator()(
        const NoDefaultTime& left,
        const NoDefaultTime& right
    ) const{
        return left.value < right.value;
    }
};

void self_test(){
    Queue queue;
    check_queue(queue, {});
    queue.insert_push(10, 5);
    queue.insert_push(30, 2);
    queue.insert_pop(40);
    assert(queue.top() == 5);
    queue.insert_push(20, 1);
    assert(queue.top() == 2);
    assert(queue.operation_count() == 4);
    assert(queue.contains_time(20));
    assert(queue.operation_kind(20) == RetroactivePriorityQueueOperationKind::push);
    assert(queue.operation_kind(40) == RetroactivePriorityQueueOperationKind::pop);
    assert(queue.pushed_value(20) == 1);
    assert(throws_runtime_error([&]{ (void)queue.pushed_value(40); }));
    assert(throws_runtime_error([&]{ (void)queue.operation_kind(99); }));

    const auto before = queue.values();
    const auto before_count = queue.operation_count();
    assert(throws_runtime_error([&]{ queue.insert_pop(0); }));
    assert(throws_runtime_error([&]{ queue.insert_push(10, 100); }));
    assert(throws_runtime_error([&]{ queue.erase(99); }));
    assert(queue.values() == before && queue.operation_count() == before_count);

    queue.erase(40);
    assert((queue.values() == std::vector<int>{1, 2, 5}));
    queue.insert_pop(40);
    queue.erase(20);
    assert((queue.values() == std::vector<int>{5}));

    PartiallyRetroactivePriorityQueue<int, int, 4> guarded_queue;
    guarded_queue.insert_push(10, 7);
    guarded_queue.insert_pop(20);
    assert(throws_runtime_error([&]{ guarded_queue.erase(10); }));
    assert(guarded_queue.contains_time(10));
    queue.clear();
    check_queue(queue, {});

    PartiallyRetroactivePriorityQueue<int, int, 2> capacity_queue;
    capacity_queue.insert_push(0, 1);
    capacity_queue.insert_push(1, 2);
    assert(throws_runtime_error([&]{ capacity_queue.insert_pop(2); }));
    assert(capacity_queue.operation_count() == 2);
    PartiallyRetroactivePriorityQueue<int, int, 0> zero_capacity;
    assert(zero_capacity.capacity() == 0);
    assert(zero_capacity.empty());
    assert(throws_runtime_error([&]{ zero_capacity.insert_push(0, 1); }));
    assert(throws_runtime_error([&]{ zero_capacity.insert_pop(0); }));

    PartiallyRetroactivePriorityQueue<int, int, 8, std::greater<int>> maximum_queue;
    maximum_queue.insert_push(0, 3);
    maximum_queue.insert_push(1, 10);
    maximum_queue.insert_pop(2);
    assert(maximum_queue.top() == 3);

    PartiallyRetroactivePriorityQueue<int, NoDefault, 8, NoDefaultLess> no_default;
    no_default.insert_push(0, NoDefault(7));
    no_default.insert_pop(1);
    assert(no_default.empty());

    PartiallyRetroactivePriorityQueue<
        NoDefaultTime,
        NoDefault,
        8,
        NoDefaultLess,
        NoDefaultTimeLess
    > no_default_time;
    no_default_time.insert_push(NoDefaultTime(0), NoDefault(4));
    assert(no_default_time.top().value == 4);

    PartiallyRetroactivePriorityQueue<
        int, int, 8, std::less<int>, std::greater<int>
    > reverse_time;
    reverse_time.insert_push(30, 5);
    reverse_time.insert_push(20, 1);
    reverse_time.insert_pop(10);
    assert(reverse_time.top() == 5);

    std::mt19937_64 random(2026071613);
    Queue random_queue;
    std::map<long long, std::optional<int>> history;
    for(int operation = 0; operation < 3000; ++operation){
        const int kind = static_cast<int>(random() % 3);
        const long long time = static_cast<long long>(random() % 5000);
        const int push_value = static_cast<int>(random() % 2001) - 1000;
        auto candidate = history;
        bool structural_valid = true;
        if(kind == 0){
            if(candidate.contains(time)) structural_valid = false;
            else candidate[time] = push_value;
        }else if(kind == 1){
            if(candidate.contains(time)) structural_valid = false;
            else candidate[time] = std::nullopt;
        }else{
            if(candidate.erase(time) == 0) structural_valid = false;
        }
        const auto replayed = structural_valid ? replay_naive(candidate) : std::nullopt;
        const bool should_succeed = structural_valid && replayed.has_value();

        bool succeeded = true;
        try{
            if(kind == 0) random_queue.insert_push(time, push_value);
            else if(kind == 1) random_queue.insert_pop(time);
            else random_queue.erase(time);
        }catch(const std::runtime_error&){
            succeeded = false;
        }
        assert(succeeded == should_succeed);
        if(should_succeed) history.swap(candidate);
        const auto expected = replay_naive(history);
        assert(expected.has_value());
        assert(random_queue.operation_count() == history.size());
        check_queue(random_queue, *expected);
    }
}

void print_state(const Queue& queue, bool success){
    std::cout << (success ? "OK" : "ERROR") << ' ' << queue.size() << ' ';
    if(queue.empty()) std::cout << "EMPTY\n";
    else std::cout << queue.top() << '\n';
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }
    Queue queue;
    for(int query = 0; query < query_count; ++query){
        char type;
        long long time;
        std::cin >> type >> time;
        bool success = true;
        try{
            if(type == 'P'){
                int value;
                std::cin >> value;
                queue.insert_push(time, value);
            }else if(type == 'O'){
                queue.insert_pop(time);
            }else{
                queue.erase(time);
            }
        }catch(const std::runtime_error&){
            success = false;
        }
        print_state(queue, success);
    }
}
