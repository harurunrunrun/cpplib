---
title: Max Plus Convolution (max-plus 畳み込み)
documentation_of: ../../../../src/algorithm/fft/min_plus/max_plus_convolution.hpp
---

$C_k = \max_{i + j = k} (A_i + B_j)$ を求める。入力は `std::vector<long long>`、出力は正確な値を保持する `std::vector<__int128_t>` である。出力の長さは $\lvert A \rvert + \lvert B \rvert - 1$ である。

| 関数 | 第 1 引数 | 第 2 引数 |
| --- | --- | --- |
| `max_plus_convolution_concave_arbitrary` | 凹 | 任意 |
| `max_plus_convolution_convex_arbitrary` | 凸 | 任意 |
| `max_plus_convolution_concave_concave` | 凹 | 凹 |

```cpp
std::vector<__int128_t> max_plus_convolution_concave_arbitrary(
    const std::vector<long long>& concave, const std::vector<long long>& arbitrary);
std::vector<__int128_t> max_plus_convolution_convex_arbitrary(
    const std::vector<long long>& convex, const std::vector<long long>& arbitrary);
std::vector<__int128_t> max_plus_convolution_concave_concave(
    const std::vector<long long>& left, const std::vector<long long>& right);
```

## 時間計算量

入力長の合計を $S$ とする。

- 凹・任意: 範囲外を大きなペナルティで埋めた全体行列の行最小値を、128 bit の評価関数と SMAWK で求める。$O(S)$
- 凹・凹: 隣接差分を大きい順にマージする。$O(S)$
- 凸・任意: 有効領域を長方形に分割し、各長方形で monotone minima を使う。$O(S \log^2 S)$

追加の空間計算量はいずれも $O(S)$ である。

## 注意点

- GCC 13 / C++20 を想定する（`__int128_t` を使う）。
- 凸・凹は、それぞれ隣接差分が広義単調増加・広義単調減少であることを指す。条件を満たさなければ `std::invalid_argument` を送出する。
- 空列との畳み込みは空列になる。
- 各入力長と出力長は `int` に収まる必要があり、超えると `std::length_error` を送出する。
- 入力の符号反転や和・差を 64 bit で計算せず、`LLONG_MIN` を含めて 128 bit で比較・出力する。
