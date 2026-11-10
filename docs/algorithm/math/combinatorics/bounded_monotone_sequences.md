---
title: Bounded Monotone Sequence Count (上下限付き単調整数列の数え上げ) [number_of_increasing_sequences_between_two_sequences]
documentation_of: ../../../../src/algorithm/math/combinatorics/bounded_monotone_sequences.hpp
---

## `count_bounded_monotone_sequences`

```cpp
Modint998244353 count_bounded_monotone_sequences(
    const vector<int>& lower,
    const vector<int>& upper,
    bool strictly_increasing = false
)
```

各位置 $i$ で `lower[i] <= x[i] < upper[i]` を満たす広義単調増加の整数列 $x$ の個数を数え、`Modint998244353` で返す。`strictly_increasing = true` のときは狭義単調増加列を数える。負の上下限、単調でない上下限、空の配列を許す。空の配列に対する答えは $1$ である。

## 時間計算量

長さを $N$、有効な上下限の値域の幅を $W$ とする。境界に沿って格子路 DP を分解し、畳み込みで遷移する。時間 $O((N+W)\log^2(N+W+1))$、空間 $O((N+W)\log(N+W+1))$。

## 注意点

上下限は同じ長さの `vector<int>` で与える。長さが異なる場合は `invalid_argument` を送出する。条件が矛盾するときは $0$ を返す。狭義の場合は `x[i] - i` に変換して同じアルゴリズムを使う。変換後の $N + W$ が $10^6$ を超える場合は `length_error` を送出する。

内部の格子路アルゴリズムは Apache-2.0 ライセンスの参照実装を改変したものである。ヘッダの帰属表記と末尾の利用条件を参照すること。
