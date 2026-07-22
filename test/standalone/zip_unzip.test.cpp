// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/other/utility/zip_unzip.hpp"

void self_test(){
    std::vector<int> data = {1, 1, 1, 2, 2, 3, 1, 1};
    auto zipped = run_length_zip(data);
    std::vector<std::pair<int, int>> expected = {{1, 3}, {2, 2}, {3, 1}, {1, 2}};
    assert(zipped == expected);
    assert(run_length_unzip(zipped) == data);
    assert(run_length_zip(std::vector<int>{}).empty());
    assert(run_length_unzip(std::vector<std::pair<int, int>>{{3, 0}}).empty());
    bool thrown = false;
    try{
        (void)run_length_unzip(std::vector<std::pair<int, int>>{{3, -1}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n;
    if(!(std::cin >> n)){
        self_test();
        return 0;
    }
    std::vector<int> data(static_cast<std::size_t>(n));
    for(int& x: data) std::cin >> x;
    auto zipped = run_length_zip(data);
    std::cout << zipped.size() << '\n';
    for(auto [value, count]: zipped){
        std::cout << value << ' ' << count << '\n';
    }
    auto restored = run_length_unzip(zipped);
    for(int i = 0; i < static_cast<int>(restored.size()); i++){
        if(i) std::cout << ' ';
        std::cout << restored[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
}
