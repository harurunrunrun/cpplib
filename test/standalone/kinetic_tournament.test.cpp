// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/kinetic_tournament.hpp"

using Tournament = KineticTournament<long long, 32768>;

static std::optional<Tournament::Result> naive(
    const std::vector<std::optional<Tournament::Line>>& lines,
    std::size_t left,
    std::size_t right,
    long long time
){
    std::optional<Tournament::Result> answer;
    for(std::size_t index = left; index < right; ++index){
        if(!lines[index]) continue;
        const auto line = *lines[index];
        const long long value = line.slope * time + line.intercept;
        if(!answer || value < answer->value
            || (value == answer->value && line.slope < answer->line.slope)
            || (value == answer->value && line.slope == answer->line.slope
                && index < answer->index)){
            answer = Tournament::Result{value, line, index};
        }
    }
    return answer;
}

static void test_random(){
    std::mt19937_64 random(2026071621ULL);
    for(int iteration = 0; iteration < 70; ++iteration){
        const std::size_t size = 1 + random() % 80;
        Tournament tournament(size, -1000);
        std::vector<std::optional<Tournament::Line>> lines(size);
        long long time = -1000;
        for(int step = 0; step < 5000; ++step){
            const int operation = static_cast<int>(random() % 10);
            if(operation < 4){
                const std::size_t index = random() % size;
                const long long slope = static_cast<long long>(random() % 101) - 50;
                const long long intercept = static_cast<long long>(random() % 20001) - 10000;
                tournament.set(index, slope, intercept);
                lines[index] = Tournament::Line{slope, intercept};
                assert(tournament.active(index));
                assert(tournament.line(index).has_value());
            }else if(operation == 4){
                const std::size_t index = random() % size;
                tournament.erase(index);
                lines[index] = std::nullopt;
            }else if(operation < 7){
                time += static_cast<long long>(random() % 20);
                tournament.advance_time(time);
                assert(tournament.current_time() == time);
            }else{
                std::size_t left = random() % (size + 1);
                std::size_t right = random() % (size + 1);
                if(left > right) std::swap(left, right);
                const auto expected = naive(lines, left, right, time);
                const auto result = tournament.query(left, right);
                assert(result.has_value() == expected.has_value());
                if(result){
                    assert(result->value == expected->value);
                    assert(result->index == expected->index);
                }
            }
            const auto expected = naive(lines, 0, size, time);
            const auto result = tournament.best();
            assert(result.has_value() == expected.has_value());
            if(result){
                assert(result->value == expected->value);
                assert(result->index == expected->index);
            }
        }
    }
}

static void test_maximum_and_errors(){
    KineticTournament<long long, 4, false> maximum(3, 0);
    maximum.set(0, 2, 10);
    maximum.set(1, 4, 1);
    maximum.set(2, -1, 20);
    assert(maximum.best()->index == 2);
    maximum.advance_time(5);
    assert(maximum.best()->index == 1);

    bool thrown = false;
    try{ maximum.advance_time(4); }catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)maximum.query(2, 4); }catch(const std::out_of_range&){ thrown = true; }
    assert(thrown);
    const long long low = std::numeric_limits<long long>::lowest();
    const long long high = std::numeric_limits<long long>::max();
    KineticTournament<long long, 2> extreme_difference(2, low);
    extreme_difference.set(0, high, low);
    extreme_difference.set(1, low, high);
    extreme_difference.advance_time(0);
    assert(extreme_difference.best()->index == 0);
    assert(extreme_difference.best()->value == low);
    extreme_difference.advance_time(1);
    assert(extreme_difference.best()->index == 1);
    assert(extreme_difference.best()->value == -1);


    KineticTournament<long long, 1> overflow(1, std::numeric_limits<long long>::max());
    overflow.set(0, 2, 0);
    thrown = false;
    try{ (void)overflow.best(); }catch(const std::overflow_error&){ thrown = true; }
    assert(thrown);

    KineticTournament<long long, 0> empty(0);
    assert(!empty.best());
}

static void test_large(){
    Tournament tournament(32768);
    for(std::size_t index = 0; index < tournament.size(); ++index){
        tournament.set(index, static_cast<long long>(index % 997),
            static_cast<long long>(tournament.size() - index));
    }
    for(long long time = 0; time <= 20000; time += 17){
        tournament.advance_time(time);
        assert(tournament.best().has_value());
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t size, query_count;
    long long initial_time;
    if(!(std::cin >> size >> query_count >> initial_time)){
        test_random();
        test_maximum_and_errors();
        test_large();
        return 0;
    }

    Tournament tournament(size, initial_time);
    while(query_count-- > 0){
        char operation;
        std::cin >> operation;
        if(operation == 'S'){
            std::size_t index;
            long long slope, intercept;
            std::cin >> index >> slope >> intercept;
            tournament.set(index, slope, intercept);
        }else if(operation == 'E'){
            std::size_t index;
            std::cin >> index;
            tournament.erase(index);
        }else if(operation == 'T'){
            long long time;
            std::cin >> time;
            tournament.advance_time(time);
        }else if(operation == 'Q'){
            std::size_t left, right;
            std::cin >> left >> right;
            const auto result = tournament.query(left, right);
            if(result) std::cout << result->value << ' ' << result->index << '\n';
            else std::cout << "EMPTY\n";
        }else{
            throw std::runtime_error("unknown operation");
        }
    }
}
