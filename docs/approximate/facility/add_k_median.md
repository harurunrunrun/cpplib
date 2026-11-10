---
title: ADD k-Median (ADD法によるk-メディアン)
documentation_of: ../../../src/approximate/facility/add_k_median.hpp
---

候補施設を 1 個ずつ追加し、各段階で全需要点への距離の和が最小になる施設を選ぶ。距離行列は `distance[facility][client]` とする。以下、候補施設数を $F$、需要点数を $C$、開設する施設数を $k$ とする。

## `add_k_median(distance, k)`

選んだ施設、各需要点の割当先、距離の和を `KMedianResult` で返す。目的値が同じなら施設番号が小さい候補を選ぶ。

- 時間計算量: $O(kFC)$
- 追加空間計算量: $O(FC+C+F)$（検査済みの行列を含む）

## 注意点

- `1 <= k <= F` でなければならない。
- 距離行列は `distance[facility][client]` 形式の長方形で、全要素が有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 一般の距離行列に対する近似比は保証しない。
