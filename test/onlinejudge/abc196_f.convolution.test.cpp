// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc196/tasks/abc196_f

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/fft/convolution.hpp"

int main(){
    using mint = Modint<998244353>;
    std::string s, t;
    std::cin >> s >> t;
    const int n = static_cast<int>(s.size());
    const int m = static_cast<int>(t.size());
    std::vector<mint> s_zero(static_cast<std::size_t>(n));
    std::vector<mint> s_one(static_cast<std::size_t>(n));
    std::vector<mint> t_zero(static_cast<std::size_t>(m));
    std::vector<mint> t_one(static_cast<std::size_t>(m));
    for(int i = 0; i < n; ++i){
        s_zero[static_cast<std::size_t>(i)] = s[static_cast<std::size_t>(i)] == '0';
        s_one[static_cast<std::size_t>(i)] = s[static_cast<std::size_t>(i)] == '1';
    }
    for(int i = 0; i < m; ++i){
        const char c = t[static_cast<std::size_t>(m - 1 - i)];
        t_zero[static_cast<std::size_t>(i)] = c == '0';
        t_one[static_cast<std::size_t>(i)] = c == '1';
    }
    const auto one_zero = convolution_ntt(s_one, t_zero);
    const auto zero_one = convolution_ntt(s_zero, t_one);
    int answer = m;
    for(int left = 0; left + m <= n; ++left){
        const int index = left + m - 1;
        const int mismatch = one_zero[static_cast<std::size_t>(index)].val() +
            zero_one[static_cast<std::size_t>(index)].val();
        answer = std::min(answer, mismatch);
    }
    std::cout << answer << '\n';
}
