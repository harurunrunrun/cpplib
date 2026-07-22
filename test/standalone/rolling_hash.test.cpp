// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include "../../src/algorithm/string/automata/rolling_hash.hpp"

std::string random_string(std::mt19937& rng, int n){
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 5));
    return s;
}

int lcp_naive(const std::string& a, const std::string& b, int i, int j){
    int result = 0;
    while(i + result < static_cast<int>(a.size()) &&
          j + result < static_cast<int>(b.size()) &&
          a[static_cast<std::size_t>(i + result)] == b[static_cast<std::size_t>(j + result)]){
        result++;
    }
    return result;
}

void test_public_api(){
    RollingHash hash;
    assert(hash.size() == 0);
    assert(hash.base_value() == RollingHash::default_base);
    assert(hash.all() == 0);

    constexpr RollingHash::u64 custom_base = 911382323;
    hash.build("banana", custom_base);
    assert(hash.size() == 6);
    assert(hash.base_value() == custom_base);
    const RollingHash same("banana", custom_base);
    assert(hash.all() == same.all());
    assert(hash.get(1, 4) == same.get(1, 4));
    assert(hash.lcp(same, 0, 0) == 6);

    bool thrown = false;
    try{
        hash.build("x", 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)hash.get(-1, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)hash.lcp(RollingHash("banana"), 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    test_public_api();
    int case_count;
    if(std::cin >> case_count){
        while(case_count--){
            std::string s, t;
            int query_count;
            std::cin >> s >> t >> query_count;
            if(s == "-") s.clear();
            if(t == "-") t.clear();
            RollingHash hs(s), ht(t);
            while(query_count--){
                int type;
                std::cin >> type;
                if(type == 0){
                    int l, r, x;
                    std::cin >> l >> r >> x;
                    std::cout << (hs.get(l, r) == hs.get(x, x + r - l)) << '\n';
                }else if(type == 1){
                    int i, j;
                    std::cin >> i >> j;
                    std::cout << hs.lcp(ht, i, j) << '\n';
                }else{
                    int l, r, x, y;
                    std::cin >> l >> r >> x >> y;
                    std::cout << (r - l == y - x && hs.get(l, r) == ht.get(x, y)) << '\n';
                }
            }
        }
        return 0;
    }
    {
        std::string s = "abracadabra";
        RollingHash rh(s);
        assert(rh.get(0, 3) == rh.get(7, 10));
        assert(rh.get(0, 4) != rh.get(1, 5));
        assert(rh.all() == rh.get(0, static_cast<int>(s.size())));
    }

    std::mt19937 rng(20260722);
    for(int n = 0; n <= 120; n++){
        std::string s = random_string(rng, n);
        std::string t = random_string(rng, n);
        RollingHash hs(s), ht(t);
        for(int l = 0; l <= n; l++){
            for(int r = l; r <= n; r++){
                for(int x = 0; x + r - l <= n; x++){
                    bool equal = s.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l)) ==
                                 s.substr(static_cast<std::size_t>(x), static_cast<std::size_t>(r - l));
                    assert((hs.get(l, r) == hs.get(x, x + r - l)) == equal);
                }
            }
        }
        for(int i = 0; i <= n; i++){
            for(int j = 0; j <= n; j++){
                assert(hs.lcp(ht, i, j) == lcp_naive(s, t, i, j));
            }
        }
    }
}
