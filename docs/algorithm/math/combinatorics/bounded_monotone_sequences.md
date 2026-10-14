---
title: Bounded Monotone Sequence Count (上下限付き単調整数列の数え上げ) [number_of_increasing_sequences_between_two_sequences]
documentation_of: ../../../../src/algorithm/math/combinatorics/bounded_monotone_sequences.hpp
---

`count_bounded_monotone_sequences(lower,upper,strictly_increasing=false)` は各位置で
`lower[i]<=x[i]<upper[i]` を満たす広義単調増加整数列を数え、`Modint998244353` を返す。
`strictly_increasing=true` では狭義単調増加列を数える。
負の上下限、非単調な上下限、空配列を許す。空配列の答えは 1。

## 時間計算量

長さを $N$、有効な上下限の値域幅を $W$ とする。
格子路 DP を境界に沿って分解し、畳み込みで遷移する。
時間 $O((N+W)\log^2(N+W+1))$、空間 $O((N+W)\log(N+W+1))$。

## 注意点

上下限は同じ長さの `vector<int>`。長さ不一致は `invalid_argument`。
条件が矛盾するときは 0。狭義の場合は `x[i]-i` へ変換して同じアルゴリズムを使う。
変換後の `N+W` が 1,000,000 を超えると `length_error`。
内部の格子路アルゴリズムは Apache-2.0 の参照実装を改変している。ヘッダの帰属表記と末尾の利用条件を参照。
