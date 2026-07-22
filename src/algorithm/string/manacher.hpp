#ifndef CPPLIB_SRC_ALGORITHM_STRING_MANACHER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_MANACHER_HPP_INCLUDED

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

std::vector<int> manacher_odd(const std::string& s){
    const int n = (int)s.size();
    std::vector<int> d(n);
    int l = 0, r = -1;
    for(int i = 0; i < n; i++){
        int k = i > r ? 1 : std::min(d[l + r - i], r - i + 1);
        while(0 <= i - k && i + k < n && s[i - k] == s[i + k]){
            k++;
        }
        d[i] = k;
        if(r < i + k - 1){
            l = i - k + 1;
            r = i + k - 1;
        }
    }
    return d;
}

std::vector<int> manacher_even(const std::string& s){
    const int n = (int)s.size();
    std::vector<int> d(n);
    int l = 0, r = -1;
    for(int i = 0; i < n; i++){
        int k = i > r ? 0 : std::min(d[l + r - i + 1], r - i + 1);
        while(0 <= i - k - 1 && i + k < n && s[i - k - 1] == s[i + k]){
            k++;
        }
        d[i] = k;
        if(r < i + k - 1){
            l = i - k;
            r = i + k - 1;
        }
    }
    return d;
}

std::pair<int, int> longest_palindrome(const std::string& s){
    const int n = (int)s.size();
    auto odd = manacher_odd(s);
    auto even = manacher_even(s);
    int best_l = 0, best_len = 0;
    for(int i = 0; i < n; i++){
        const int odd_len = 2 * odd[i] - 1;
        if(best_len < odd_len){
            best_len = odd_len;
            best_l = i - odd[i] + 1;
        }
        const int even_len = 2 * even[i];
        if(best_len < even_len){
            best_len = even_len;
            best_l = i - even[i];
        }
    }
    return {best_l, best_len};
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_MANACHER_HPP_INCLUDED
