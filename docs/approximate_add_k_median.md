---
title: ADD法によるk-median
documentation_of: ../src/approximate/facility/add_k_median.hpp
---

# ADD法によるk-median

候補施設を1個ずつ追加し、各段階で全需要点への距離和が最小になる施設を選ぶ。距離行列は `distance[facility][client]` とする。

## `add_k_median(distance, k)`

選んだ施設、各需要点の割当先、距離和を `KMedianResult` で返す。同じ目的値なら施設番号が小さい候補を選ぶ。

- 時間計算量: $O(kFC)$
- 追加空間計算量: $O(FC+C+F)$（検査済み行列を含む）
- 近似: 一般の距離行列に対する近似比は保証しない。

`1 <= k <= F` が必要である。行列は長方形で、全要素が有限かつ非負でなければならない。
