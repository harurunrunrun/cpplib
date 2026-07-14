// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/fft/min_plus_convolution_smawk.hpp"

void self_test(){
    assert(min_plus_convolution_convex_arbitrary_smawk<int>({}, {1}).empty());
    assert((min_plus_convolution_convex_arbitrary_smawk<int>(
        {0, 1, 4}, {3, -2}
    ) == std::vector<int>{3, -2, -1, 2}));

    bool thrown = false;
    try{
        (void)min_plus_convolution_convex_arbitrary_smawk<int>(
            {0, 3, 1}, {0}
        );
    }catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)min_plus_convolution_convex_arbitrary_smawk<long long>(
            {std::numeric_limits<long long>::max()}, {1}
        );
    }catch(const std::overflow_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    self_test();
    int convex_size, arbitrary_size;
    if(!(std::cin >> convex_size >> arbitrary_size)) return 0;
    std::vector<long long> convex(static_cast<std::size_t>(convex_size));
    std::vector<long long> arbitrary(static_cast<std::size_t>(arbitrary_size));
    for(long long& value: convex) std::cin >> value;
    for(long long& value: arbitrary) std::cin >> value;
    const auto answer = min_plus_convolution_convex_arbitrary_smawk(
        convex, arbitrary
    );
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
