---
title: ADD-DROP k-Median (ADD-DROP法によるk-メディアン)
documentation_of: ../src/approximate/facility/add_drop_k_median.hpp
---

# ADD-DROP法によるk-median

ADD法で初期解を作り、閉施設を1個追加して開施設を1個除く交換を反復する。

## `add_drop_k_median(distance, k, maximum_iterations)`

`KMedianResult` を返す。改善する交換だけを採用し、同値なら施設列が辞書順最小の解を選ぶ。

- 時間計算量: $O(kFC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$
- 近似: 一般の距離行列に対する近似比は保証しない。

`maximum_iterations == 0` ではADD初期解を返す。
