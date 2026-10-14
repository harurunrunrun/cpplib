---
title: Max Plus Convolution (max-plus 畳み込み)
documentation_of: ../../../../src/algorithm/fft/min_plus/max_plus_convolution.hpp
---

`C[k]=max_(i+j=k)(A[i]+B[j])` を求める。入力は `vector<long long>`、正確な出力は `vector<__int128_t>`。

| 関数 | 先頭列 | 第2列 |
| --- | --- | --- |
| `max_plus_convolution_concave_arbitrary` | 凹 | 任意 |
| `max_plus_convolution_convex_arbitrary` | 凸 | 任意 |
| `max_plus_convolution_concave_concave` | 凹 | 凹 |

## 時間計算量

総入力長を $S$ とする。凹・任意は128 bitの評価関数とSMAWKで $O(S)$、凹・凹は傾きのマージで $O(S)$。
凸・任意は部分行列を長方形へ分割し、Monotone Minima を使うため $O(S\log^2 S)$。
追加領域はいずれも $O(S)$。

## 注意点

GCC 13 / C++20。凸・凹は隣接差分の非減少・非増加を指す。条件違反は例外。
空列との畳み込みは空列。総出力長は `int` に収まる必要がある。
入力の負号や和・差を64 bitで計算せず、`LLONG_MIN` を含めて128 bitで比較・出力する。
