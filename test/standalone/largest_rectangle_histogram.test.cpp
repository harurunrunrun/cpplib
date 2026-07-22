// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/other/sequence/largest_rectangle_histogram.hpp"

void test_basic(){
    const auto result = largest_rectangle_histogram(std::vector<long long>{2, 1, 5, 6, 2, 3});
    assert(result.area == 10);
    assert(result.left == 2);
    assert(result.right == 4);
    assert(result.height == 5);

    assert(largest_rectangle_histogram(std::vector<int>{}).area == 0);
    assert(largest_rectangle_histogram(std::vector<int>{0, 0, 0}).area == 0);
    assert(largest_rectangle_histogram(std::vector<int>{4, 4, 4}).area == 12);

    bool thrown = false;
    try{
        (void)largest_rectangle_histogram(std::vector<int>{1, -1, 2});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n;
    if(std::cin >> n){
        std::vector<long long> heights(static_cast<std::size_t>(n));
        for(auto& height: heights) std::cin >> height;
        std::cout << largest_rectangle_histogram(heights).area << '\n';
        return 0;
    }
    test_basic();
}
