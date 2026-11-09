---
title: GCD / LCM Convolution (GCD・LCM畳み込み) [gcd_convolution] [lcm_convolution]
documentation_of: ../../../../src/algorithm/fft/transform/divisor_convolution.hpp
---

正整数を添字とする GCD 畳み込みと LCM 畳み込みである。`std::vector` の添字 `i` が整数 $i + 1$ に対応する（1-indexed の値を 0-indexed の配列に格納する）。

```cpp
template<class T> std::vector<T> gcd_convolution(const std::vector<T>& a, const std::vector<T>& b);
template<class T> std::vector<T> lcm_convolution(const std::vector<T>& a, const std::vector<T>& b);

auto gcd_result = gcd_convolution(a, b);
auto lcm_result = lcm_convolution(a, b);
```

入力長を $N$ とすると、長さ $N$ の返り値は次を満たす。

$$
g_k=\sum_{\gcd(i,j)=k}a_i b_j,
\qquad
l_k=\sum_{\operatorname{lcm}(i,j)=k}a_i b_j.
$$

LCM が $N$ を超える組は返り値に含まれない。値型 `T` はデフォルト構築（零元）、加減算、乗算が使える必要がある。

## 計算量

| 操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `gcd_convolution(a, b)` | $O(N\log(N+1))$ | $O(N)$ |
| `lcm_convolution(a, b)` | $O(N\log(N+1))$ | $O(N)$ |

## 注意点

- `a` と `b` の長さが異なる場合は `std::runtime_error` を送出する。
- 空の `std::vector` も受け付け、空の `std::vector` を返す。
