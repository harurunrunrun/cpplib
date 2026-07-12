// competitive-verifier: STANDALONE

#include <cassert>
#include <random>
#include <string>
#include "../../src/algorithm/string/rolling_hash.hpp"

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

int main(){
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
