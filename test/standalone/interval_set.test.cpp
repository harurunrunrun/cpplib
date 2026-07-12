// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../../src/algorithm/range/interval_set.hpp"

bool covered_naive(const std::vector<char>& used, int l, int r){
    for(int i = l; i < r; i++){
        if(!used[static_cast<std::size_t>(i)]) return false;
    }
    return true;
}

int mex_naive(const std::vector<char>& used, int x){
    while(x < static_cast<int>(used.size()) && used[static_cast<std::size_t>(x)]) x++;
    return x;
}

void self_test(){
    IntervalSet<int> intervals;
    intervals.add(1, 4);
    intervals.add(4, 7);
    assert(intervals.size() == 1);
    assert(intervals.covered(1, 7));
    intervals.erase(3, 5);
    assert(intervals.contains(2));
    assert(!intervals.contains(3));
    assert(intervals.contains(5));
    assert(intervals.mex(1) == 3);

    std::mt19937 rng(20260908);
    for(int step = 0; step < 1000; step++){
        IntervalSet<int> set;
        std::vector<char> used(64, 0);
        for(int q = 0; q < 200; q++){
            int l = static_cast<int>(rng() % 64);
            int r = static_cast<int>(rng() % 64);
            if(l > r) std::swap(l, r);
            int type = static_cast<int>(rng() % 4);
            if(type == 0){
                set.add(l, r);
                for(int i = l; i < r; i++) used[static_cast<std::size_t>(i)] = 1;
            }else if(type == 1){
                set.erase(l, r);
                for(int i = l; i < r; i++) used[static_cast<std::size_t>(i)] = 0;
            }else if(type == 2){
                assert(set.covered(l, r) == covered_naive(used, l, r));
            }else{
                int x = l;
                assert(set.mex(x) == mex_naive(used, x));
            }
            for(int i = 0; i < 64; i++){
                assert(set.contains(i) == static_cast<bool>(used[static_cast<std::size_t>(i)]));
            }
        }
    }
}

int main(){
    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }
    IntervalSet<int> set;
    while(q--){
        std::string type;
        int l, r;
        std::cin >> type >> l >> r;
        if(type == "ADD"){
            set.add(l, r);
        }else if(type == "ERASE"){
            set.erase(l, r);
        }else if(type == "CONTAINS"){
            std::cout << set.contains(l) << '\n';
        }else if(type == "COVERED"){
            std::cout << set.covered(l, r) << '\n';
        }else if(type == "MEX"){
            std::cout << set.mex(l) << '\n';
        }
    }
}
