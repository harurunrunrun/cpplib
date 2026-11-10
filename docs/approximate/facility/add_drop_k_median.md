---
title: ADD-DROP k-Median (ADD-DROP法によるk-メディアン)
documentation_of: ../../../src/approximate/facility/add_drop_k_median.hpp
---

ADD 法で初期解を作り、閉じている施設を 1 個開いて開いている施設を 1 個閉じる交換を繰り返す。以下、候補施設数を $F$、需要点数を $C$、開設する施設数を $k$、採用した交換の回数を $I$ とする。

## `add_drop_k_median(distance, k, maximum_iterations = 100)`

`KMedianResult` を返す。改善する交換だけを採用し、改善量が同じなら施設列が辞書順最小となる解を選ぶ。`maximum_iterations == 0` では ADD 法による初期解を返す。

- 時間計算量: $O(kFC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$

## 注意点

- `1 <= k <= F` でなければならない。
- 距離行列は `distance[facility][client]` 形式の長方形で、全要素が有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 一般の距離行列に対する近似比は保証しない。
