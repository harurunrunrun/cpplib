---
title: Multiplicative Convolution Powers (乗法剰余ごとの列の重み付き数え上げ)
documentation_of: ../../../../src/algorithm/fft/transform/multiplicative_convolution_power.hpp
---

長さ $N$（素数または 2 の冪）の重み列 `a` と非負整数 $K$ を受け取り、長さ $K$ の順序付き列 $(x_1, \ldots, x_K)$ すべてについて重みの積 $a_{x_1} \cdots a_{x_K}$ を、積 $x_1 \cdots x_K \bmod N$ ごとに合計した列を返す。すなわち、乗法を添字とする畳み込みでの `a` の $K$ 乗である。

```cpp
template<int MOD = 998244353>
std::vector<Modint<MOD>> prime_multiplicative_power(std::vector<Modint<MOD>> values, std::uint64_t exponent);

template<int MOD = 998244353>
std::vector<Modint<MOD>> power_two_multiplicative_power(std::vector<Modint<MOD>> values, std::uint64_t exponent);
```

`prime_multiplicative_power` は長さが素数の場合、`power_two_multiplicative_power` は長さが 2 の冪の場合に対応する。指数の型は `std::uint64_t` である。$K = 0$ のときは空積に対応する剰余 $1 \bmod N$ の重みだけが $1$ になる。

## 計算量

繰り返し二乗法で乗法畳み込みを繰り返す。長さ $N$ に対して時間 $O(N \log N \log(K+1))$、作業領域 $O(N)$ である。

## 注意点

- 係数は `Modint<MOD>` で、デフォルトの法は $998244353$ である。
- 素数版は長さが素数でない、または $2^{31} - 1$ を超える場合に、2 の冪版は長さが 2 の冪でない、または $2^{30}$ を超える場合に `std::invalid_argument` を送出する。空の配列は受け付けない。
- 2 の冪版は `power_two_multiplicative_convolution` が扱える長さと法を使う。素数版は任意 mod 畳み込みの長さ上限に従う。
- 長さ $1$ は 2 の冪版で扱え、すべての列が剰余 $0$ に入る。
- GCC 13 / C++20 を想定する。
