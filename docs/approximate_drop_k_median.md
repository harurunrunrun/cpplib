---
title: DROP k-Median (DROP法によるk-メディアン)
documentation_of: ../src/approximate/facility/drop_k_median.hpp
---

# DROP法によるk-median

全候補を開いた状態から、削除後の距離和が最小となる施設を1個ずつ除く。

## `drop_k_median(distance, k)`

`KMedianResult` を返す。同値なら削除後の施設列が辞書順最小となる施設を除く。

- 時間計算量: $O(FC+(F-k)FC)$
- 追加空間計算量: $O(FC+C+F)$
- 近似: 一般の距離行列に対する近似比は保証しない。

各反復で最近施設と第2近傍施設を計算するため、候補ごとに割当を全探索し直さない。
