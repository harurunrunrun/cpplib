// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <set>
#include <vector>
#include "../../src/structure/bbst/persistent_red_black_tree.hpp"

std::optional<int> kth_naive(const std::set<int>& s, int k){
    if(k < 0 || static_cast<int>(s.size()) <= k) return std::nullopt;
    auto it = s.begin();
    std::advance(it, k);
    return *it;
}

void check_version(const PersistentRedBlackSet<int, 20000, 2000>& tree, int version, const std::set<int>& s){
    assert(tree.size(version) == static_cast<int>(s.size()));
    assert(tree.empty(version) == s.empty());
    for(int x = -20; x <= 120; x++){
        assert(tree.contains(x, version) == (s.find(x) != s.end()));
        assert(tree.order_of_key(x, version) == static_cast<int>(std::distance(s.begin(), s.lower_bound(x))));
        assert(tree.order_of_key_upper(x, version) == static_cast<int>(std::distance(s.begin(), s.upper_bound(x))));
    }
    for(int k = -2; k <= static_cast<int>(s.size()) + 2; k++){
        assert(tree.kth(k, version) == kth_naive(s, k));
    }
    std::vector<int> values(s.begin(), s.end());
    assert(tree.to_vector(version) == values);
}

void self_test(){
    PersistentRedBlackSet<int, 20000, 2000> tree;
    std::vector<std::set<int>> naive(1);
    std::mt19937 rng(20260916);
    for(int step = 0; step < 500; step++){
        int base = static_cast<int>(rng() % naive.size());
        int x = static_cast<int>(rng() % 101);
        int type = static_cast<int>(rng() % 3);
        int next;
        std::set<int> expected = naive[static_cast<std::size_t>(base)];
        if(type == 0){
            next = tree.insert(x, base);
            expected.insert(x);
        }else{
            next = tree.erase(x, base);
            expected.erase(x);
        }
        naive.push_back(expected);
        assert(next == static_cast<int>(naive.size()) - 1);
        check_version(tree, base, naive[static_cast<std::size_t>(base)]);
        check_version(tree, next, expected);
    }
}

int main(){
    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }
    PersistentRedBlackSet<int, 20000, 2000> tree;
    while(q--){
        std::string type;
        int v, x;
        std::cin >> type >> v >> x;
        if(type == "INSERT"){
            std::cout << tree.insert(x, v) << '\n';
        }else if(type == "ERASE"){
            std::cout << tree.erase(x, v) << '\n';
        }else if(type == "CONTAINS"){
            std::cout << tree.contains(x, v) << '\n';
        }else if(type == "KTH"){
            auto res = tree.kth(x, v);
            std::cout << (res ? *res : -1) << '\n';
        }
    }
}
