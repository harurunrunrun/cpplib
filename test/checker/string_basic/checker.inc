#include <algorithm>
#include <cassert>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../../src/algorithm/string/longest_common_substring.hpp"
#include "../../../src/algorithm/string/manacher.hpp"
#include "../../../src/algorithm/string/suffix_array.hpp"
#include "../../../src/algorithm/string/z_algorithm.hpp"

std::string binary_string(int n, int mask){
    std::string s(n, 'a');
    for(int i = 0; i < n; i++){
        if(mask >> i & 1){
            s[i] = 'b';
        }
    }
    return s;
}

std::vector<int> naive_z(const std::string& s){
    const int n = (int)s.size();
    std::vector<int> z(n);
    if(n == 0){
        return z;
    }
    z[0] = n;
    for(int i = 1; i < n; i++){
        while(i + z[i] < n && s[z[i]] == s[i + z[i]]){
            z[i]++;
        }
    }
    return z;
}

std::vector<int> naive_odd(const std::string& s){
    const int n = (int)s.size();
    std::vector<int> d(n);
    for(int i = 0; i < n; i++){
        while(i - d[i] >= 0 && i + d[i] < n && s[i - d[i]] == s[i + d[i]]){
            d[i]++;
        }
    }
    return d;
}

std::vector<int> naive_even(const std::string& s){
    const int n = (int)s.size();
    std::vector<int> d(n);
    for(int i = 0; i < n; i++){
        while(i - d[i] - 1 >= 0 && i + d[i] < n && s[i - d[i] - 1] == s[i + d[i]]){
            d[i]++;
        }
    }
    return d;
}

std::vector<int> naive_suffix_array(const std::string& s){
    std::vector<int> sa(s.size());
    std::iota(sa.begin(), sa.end(), 0);
    std::sort(sa.begin(), sa.end(), [&](int l, int r){
        return s.substr(l) < s.substr(r);
    });
    return sa;
}

std::vector<int> naive_lcp(const std::string& s, const std::vector<int>& sa){
    std::vector<int> lcp(sa.size() == 0 ? 0 : sa.size() - 1);
    for(int i = 0; i + 1 < (int)sa.size(); i++){
        while(sa[i] + lcp[i] < (int)s.size() &&
              sa[i + 1] + lcp[i] < (int)s.size() &&
              s[sa[i] + lcp[i]] == s[sa[i + 1] + lcp[i]]){
            lcp[i]++;
        }
    }
    return lcp;
}

int naive_lcs_length(const std::string& a, const std::string& b){
    int best = 0;
    for(int i = 0; i < (int)a.size(); i++){
        for(int j = 0; j < (int)b.size(); j++){
            int len = 0;
            while(i + len < (int)a.size() && j + len < (int)b.size() && a[i + len] == b[j + len]){
                len++;
            }
            best = std::max(best, len);
        }
    }
    return best;
}

void test_z_algorithm(){
    assert(z_algorithm(std::string("aabcaabxaaaz")) == std::vector<int>({12, 1, 0, 0, 3, 1, 0, 0, 2, 2, 1, 0}));
    for(int n = 0; n <= 9; n++){
        for(int mask = 0; mask < (1 << n); mask++){
            const auto s = binary_string(n, mask);
            assert(z_algorithm(s) == naive_z(s));
        }
    }
}

void test_manacher(){
    for(int n = 0; n <= 9; n++){
        for(int mask = 0; mask < (1 << n); mask++){
            const auto s = binary_string(n, mask);
            assert(manacher_odd(s) == naive_odd(s));
            assert(manacher_even(s) == naive_even(s));

            auto [l, len] = longest_palindrome(s);
            assert(0 <= l && l + len <= n);
            assert(len == 0 || std::string(s.begin() + l, s.begin() + l + len) ==
                                std::string(s.rbegin() + (n - l - len), s.rbegin() + (n - l)));
            int expected = 0;
            for(int x: naive_odd(s)){
                expected = std::max(expected, 2 * x - 1);
            }
            for(int x: naive_even(s)){
                expected = std::max(expected, 2 * x);
            }
            assert(len == expected);
        }
    }
}

void test_suffix_array(){
    const char* samples[] = {
        "",
        "a",
        "aaaaa",
        "banana",
        "mississippi",
        "abracadabra",
    };
    for(const char* p: samples){
        std::string s = p;
        auto sa = suffix_array(s);
        assert(sa == naive_suffix_array(s));
        assert(lcp_array(s, sa) == naive_lcp(s, sa));
        assert(lcp_array(s) == naive_lcp(s, sa));
    }
    {
        std::string s("a\0b\0a", 5);
        auto sa = suffix_array(s);
        assert(sa == naive_suffix_array(s));
        assert(lcp_array(s, sa) == naive_lcp(s, sa));
        assert(lcp_array(s) == naive_lcp(s, sa));
    }

    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    auto sa = suffix_array(v);
    for(int i = 0; i + 1 < (int)sa.size(); i++){
        assert(std::lexicographical_compare(v.begin() + sa[i], v.end(), v.begin() + sa[i + 1], v.end()));
    }

    bool thrown = false;
    try{
        (void)lcp_array(std::string("abc"), std::vector<int>{0, 0, 2});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_longest_common_substring(){
    {
        auto res = longest_common_substring("xabxac", "abcabxabcd");
        assert(res.length == 4);
        assert(std::string("xabxac").substr(res.first_pos, res.length) ==
               std::string("abcabxabcd").substr(res.second_pos, res.length));
    }
    const char* a[] = {"", "a", "banana", "abracadabra", "abcabc", "zzzz"};
    const char* b[] = {"", "b", "ananas", "cadabra", "bcabca", "azzzb"};
    for(const char* xp: a){
        for(const char* yp: b){
            std::string x = xp;
            std::string y = yp;
            auto res = longest_common_substring(x, y);
            assert(res.length == naive_lcs_length(x, y));
            assert(x.substr(res.first_pos, res.length) == y.substr(res.second_pos, res.length));
        }
    }
}

inline void run_string_basic_standalone_check(){
    test_z_algorithm();
    test_manacher();
    test_suffix_array();
    test_longest_common_substring();
}
