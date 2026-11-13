---
title: DROP k-Median (DROP法によるk-メディアン)
documentation_of: ../../../src/approximate/facility/drop_k_median.hpp
---

全ての候補施設を開いた状態から始め、閉じた後の距離の和が最小になる施設を 1 個ずつ閉じる。以下、候補施設数を $F$、需要点数を $C$、開設する施設数を $k$ とする。

## `drop_k_median(distance, k)`

`KMedianResult` を返す。同値なら、削除後の施設列が辞書順最小となる施設を閉じる。各反復で各需要点の最も近い施設と 2 番目に近い施設を計算しておくため、候補ごとに割当を全探索し直すことはない。

- 時間計算量: $O(FC+(F-k)FC)$
- 追加空間計算量: $O(FC+C+F)$

## 注意点

- `1 <= k <= F` でなければならない。
- 距離行列は `distance[facility][client]` 形式の長方形で、全要素が有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 一般の距離行列に対する近似比は保証しない。
