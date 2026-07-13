// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/convex_hull_trick/segment_li_chao_tree.hpp"

struct StoredSegment{
    long long left;
    long long right;
    long long slope;
    long long intercept;
};

std::optional<long long> naive_query(
    const std::vector<StoredSegment>& segments,
    long long x
){
    std::optional<long long> answer;
    for(const auto& segment: segments){
        if(segment.left <= x && x < segment.right){
            const long long value = segment.slope * x + segment.intercept;
            if(!answer || value < *answer) answer = value;
        }
    }
    return answer;
}

void test_random(){
    std::vector<long long> coordinates;
    for(long long x = -60; x <= 60; ++x) coordinates.push_back(x);
    SegmentLiChaoTree tree(coordinates);
    std::vector<StoredSegment> segments;
    std::mt19937_64 rng(2026071451);
    for(int step = 0; step < 10000; ++step){
        if(rng() % 3 != 0){
            long long left = static_cast<long long>(rng() % 141) - 70;
            long long right = static_cast<long long>(rng() % 141) - 70;
            if(left > right) std::swap(left, right);
            const long long slope = static_cast<long long>(rng() % 101) - 50;
            const long long intercept = static_cast<long long>(rng() % 2001) - 1000;
            tree.add_segment(left, right, slope, intercept);
            segments.push_back({left, right, slope, intercept});
        }else{
            const long long x = coordinates[static_cast<std::size_t>(rng() % coordinates.size())];
            assert(tree.query(x) == naive_query(segments, x));
        }
    }
    for(long long x: coordinates) assert(tree.query(x) == naive_query(segments, x));
}

void test_boundaries_and_errors(){
    SegmentLiChaoTree tree({5, -2, 5, 0, 10});
    assert(tree.size() == 4);
    assert(tree.empty());
    assert(!tree.query(-2));
    tree.add_segment(-2, 5, 3, 7);
    assert(tree.query(-2) == 1);
    assert(tree.query(0) == 7);
    assert(!tree.query(5));
    tree.add_line(-1, 4);
    assert(tree.query(-2) == 1);
    assert(tree.query(5) == -1);
    assert(tree.query(10) == -6);
    tree.add_segment(0, 0, -100, -100);
    assert(tree.query(0) == 4);

    bool thrown = false;
    try{ (void)tree.query(3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ tree.add_segment(4, 3, 0, 0); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    SegmentLiChaoTree no_coordinates({});
    no_coordinates.add_line(1, 2);
    no_coordinates.add_segment(-5, 8, 3, 4);
    assert(no_coordinates.empty());
}

struct Operation{
    std::string type;
    long long first;
    long long second;
    long long third;
    long long fourth;
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int operation_count;
    if(!(std::cin >> operation_count)){
        test_random();
        test_boundaries_and_errors();
        return 0;
    }
    std::vector<Operation> operations;
    std::vector<long long> coordinates;
    operations.reserve(static_cast<std::size_t>(operation_count));
    for(int operation = 0; operation < operation_count; ++operation){
        std::string type;
        std::cin >> type;
        if(type == "ADD"){
            long long left, right, slope, intercept;
            std::cin >> left >> right >> slope >> intercept;
            operations.push_back({type, left, right, slope, intercept});
        }else if(type == "LINE"){
            long long slope, intercept;
            std::cin >> slope >> intercept;
            operations.push_back({type, slope, intercept, 0, 0});
        }else{
            long long x;
            std::cin >> x;
            operations.push_back({type, x, 0, 0, 0});
            coordinates.push_back(x);
        }
    }
    SegmentLiChaoTree tree(std::move(coordinates));
    for(const auto& operation: operations){
        if(operation.type == "ADD"){
            tree.add_segment(
                operation.first, operation.second, operation.third, operation.fourth
            );
        }else if(operation.type == "LINE"){
            tree.add_line(operation.first, operation.second);
        }else{
            const auto answer = tree.query(operation.first);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "INFINITY\n";
        }
    }
}
